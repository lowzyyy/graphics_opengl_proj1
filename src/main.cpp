#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <rg/coordinate_system.h>
#include <rg/lightSourceCube.h>
#include <rg/water.h>
#include <rg/waterFrameBuffers.h>
#include <rg/gui_picture.h>
#include <iostream>
void CenterTheWindow(GLFWwindow* window,int m_Width,int m_Height){
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwSetWindowPos(window, (mode->width - m_Width) / 2, (mode->height - m_Height) / 2);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// camera

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct ProgramState {
    glm::vec3 clearColor = glm::vec3(0);
    bool ImGuiEnabled = false;
    bool showSmallMaps = false;
    bool wireFrameOption = false;
    float lightPos3f[3];
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;
    glm::vec3 backpackPosition = glm::vec3(0.0f);
    float backpackScale = 1.0f;
    PointLight pointLight;
    PointLight pointLight_lamp;
    ProgramState()
            : camera(glm::vec3(0.0f, 25.0f, -10.0f)) {}

    void SaveToFile(std::string filename);

    void LoadFromFile(std::string filename);
};

void ProgramState::SaveToFile(std::string filename) {
    std::ofstream out(filename);
    out << clearColor.r << '\n'
        << clearColor.g << '\n'
        << clearColor.b << '\n'
        << ImGuiEnabled << '\n'
        << camera.Position.x << '\n'
        << camera.Position.y << '\n'
        << camera.Position.z << '\n'
        << camera.Front.x << '\n'
        << camera.Front.y << '\n'
        << camera.Front.z << '\n';
}

void ProgramState::LoadFromFile(std::string filename) {
    std::ifstream in(filename);
    if (in) {
        in >> clearColor.r
           >> clearColor.g
           >> clearColor.b
           >> ImGuiEnabled
           >> camera.Position.x
           >> camera.Position.y
           >> camera.Position.z
           >> camera.Front.x
           >> camera.Front.y
           >> camera.Front.z;
    }
}

ProgramState *programState;

void DrawImGui(ProgramState *programState);

void renderScene(Model& lampModel, Shader& lampShader,
                 Model& ourModel,Shader& ourShader,
                 lightSourceCube &lightCube1,
                 coordinate_system& coord_system,
                 glm::vec3& lightPosition,
                 glm::vec4 clipPlane){
    ourShader.use();
    ourShader.setVec4("plane",clipPlane);
    ourModel.Draw(ourShader);
    lampShader.use();
    lampShader.setVec4("plane",clipPlane);
    lampModel.Draw(lampShader);
    lightCube1.mPlane = clipPlane;
    lightCube1.draw(programState->camera,SCR_WIDTH,SCR_HEIGHT);
//    lightCube1.setLightPosition(glm::vec3(programState->lightPos3f[0], programState->lightPos3f[1], programState->lightPos3f[2]));
//    lightCube1.draw(programState->camera,SCR_WIDTH,SCR_HEIGHT);
    coord_system.mPlane = clipPlane;
    coord_system.draw(programState->camera,SCR_WIDTH,SCR_HEIGHT);
}
int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    programState = new ProgramState;
//    programState->LoadFromFile("resources/program_state.txt");
    if (programState->ImGuiEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;



    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    CenterTheWindow(window,SCR_WIDTH,SCR_HEIGHT);
    //glfwSetWindowPos(window,SCR_WIDTH/2,SCR_HEIGHT/2);
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("resources/shaders/modelVertexShader.vs", "resources/shaders/modelFragmentShader.fs");
    Shader lampShader("resources/shaders/modelVertexShader.vs", "resources/shaders/modelFragmentShader.fs");
    Shader coordinateSystemShader("resources/shaders/vertexShader_coordinate_system.vs","resources/shaders/fragmentShader_coordinate_system.fs");
    Shader lightSourceShader("resources/shaders/lightSourceVertexShader.vs", "resources/shaders/lightSourceFragmentShader.fs");
    Shader waterShader("resources/shaders/waterVertexShader.vs","resources/shaders/waterFragmentShader.fs");
    Shader smallMapShader("resources/shaders/smallReflectionMapVertexShader.vs","resources/shaders/smallReflectionMapFragmentShader.fs");
    // load models
    // -----------
    Model ourModel("resources/objects/ostrvo2/Small Tropical Island/Small Tropical Island.obj");
    Model lampModel("resources/objects/lampa1/Street Lamp/StreetLamp.obj");
    ourModel.SetShaderTextureNamePrefix("material.");



    PointLight& pointLight = programState->pointLight;
    pointLight.position = glm::vec3(4.0f, 5, 0.0);
    pointLight.ambient = glm::vec3(0.1, 0.1, 0.1);
    pointLight.diffuse = glm::vec3(0.6, 0.6, 0.6);
    pointLight.specular = glm::vec3(1.0, 1.0, 1.0);

    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;
    pointLight.quadratic = 0.032f;
    
    PointLight& pointLight_lamp = programState->pointLight_lamp;
    programState->lightPos3f[0]=100;
    programState->lightPos3f[1]=5;
    programState->lightPos3f[2]=100;
    pointLight_lamp.position = glm::vec3(programState->lightPos3f[0], programState->lightPos3f[1], programState->lightPos3f[2]);
    pointLight_lamp.ambient = glm::vec3(0.1, 0.1, 0.1);
    pointLight_lamp.diffuse = glm::vec3(0.6, 0.6, 0.6);
    pointLight_lamp.specular = glm::vec3(1.0, 1.0, 1.0);

    pointLight_lamp.constant = 1.0f;
    pointLight_lamp.linear = 0.09f;
    pointLight_lamp.quadratic = 0.032f;

    float water_height = 0.0;
    glm::vec4 neutralClipPlane= glm::vec4(0.0,0.0,0.0,water_height);
    coordinate_system coord_system(neutralClipPlane,coordinateSystemShader);
    lightSourceCube lightCube1(neutralClipPlane, lightSourceShader);
    water terrain_water(200.0,water_height,neutralClipPlane);
    waterFrameBuffers water_FBO(SCR_WIDTH,SCR_HEIGHT);
    gui_picture smallReflectionMap(0.5,0.5,true);
    gui_picture smallRefractionMap(0.5,-0.5,false);

    // draw in wireframe
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glEnable(GL_CLIP_DISTANCE0);

    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if(programState->wireFrameOption)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // input
        // -----
        processInput(window);


        // render
        // ------
        glClearColor(programState->clearColor.r, programState->clearColor.g, programState->clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        ---------------------------------ISLAND-------------------------------
        ourShader.use();
        pointLight.position = glm::vec3(10.0 * cos(currentFrame), 25.0f, 10.0 * sin(currentFrame));
        ourShader.setVec3("pointLight.position", pointLight.position);
        ourShader.setVec3("pointLight.ambient", pointLight.ambient);
        ourShader.setVec3("pointLight.diffuse", pointLight.diffuse);
        ourShader.setVec3("pointLight.specular", pointLight.specular);
        ourShader.setFloat("pointLight.constant", pointLight.constant);
        ourShader.setFloat("pointLight.linear", pointLight.linear);
        ourShader.setFloat("pointLight.quadratic", pointLight.quadratic);
        ourShader.setVec3("viewPosition", programState->camera.Position);
        ourShader.setFloat("material.shininess", 8.0f);
        ourShader.setVec4("plane",neutralClipPlane);
        // view/projection/model transformations
        glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 500.0f);
        ourShader.setMat4("projection", projection);
        glm::mat4 view = programState->camera.GetViewMatrix();
        ourShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0,-2,0.0)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.5));    // it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
//        ---------------------------LAMP---------------------------
        lampShader.use();
        lampShader.setVec3("pointLight.position", glm::vec3(programState->lightPos3f[0],programState->lightPos3f[1],programState->lightPos3f[2]));
        lampShader.setVec3("pointLight.ambient", pointLight_lamp.ambient);
        lampShader.setVec3("pointLight.diffuse", pointLight_lamp.diffuse);
        lampShader.setVec3("pointLight.specular", pointLight_lamp.specular);
        lampShader.setFloat("pointLight.constant", pointLight_lamp.constant);
        lampShader.setFloat("pointLight.linear", pointLight_lamp.linear);
        lampShader.setFloat("pointLight.quadratic", pointLight_lamp.quadratic);
        lampShader.setVec3("viewPosition", programState->camera.Position);
        lampShader.setFloat("material.shininess", 8.0f);
        lampShader.setVec4("plane",neutralClipPlane);
        glm::mat4 projection_lamp = glm::perspective(glm::radians(programState->camera.Zoom),
                                                (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 500.0f);
        glm::mat4 view_lamp = programState->camera.GetViewMatrix();
        ourShader.setMat4("view", view_lamp);
        glm::mat4 model_lamp = glm::mat4(1.0);
        model_lamp = glm::translate(model_lamp,glm::vec3(100.0,-1.0,100.0));
        model_lamp = glm::rotate(model_lamp, glm::radians(135.0f),glm::vec3(0.0,1.0,0.0));
        lampShader.setMat4("model",model_lamp);
        lampShader.setMat4("projection",projection_lamp);
        lampShader.setMat4("view",view_lamp);

//        ----------------------------------------- RENDER -----------------------
        water_FBO.bindReflectionFrameBuffer();
        programState->camera.invertPitch();
        programState->camera.invertY();
        view = programState->camera.GetViewMatrix();
        ourShader.use();
        ourShader.setMat4("view", view);
        lampShader.use();
        lampShader.setMat4("view", view);
        lightCube1.setLightPosition(pointLight.position);
        renderScene(lampModel,lampShader,ourModel, ourShader, lightCube1, coord_system,pointLight.position,glm::vec4(0.0,1.0,0.0,-water_height));
        programState->camera.invertPitch();
        programState->camera.invertY();
        water_FBO.unbindCurrentBuffer();


        view = programState->camera.GetViewMatrix();
        ourShader.use();
        ourShader.setMat4("view", view);
        lampShader.use();
        lampShader.setMat4("view", view);
        water_FBO.bindRefractionFrameBuffer();
        lightCube1.setLightPosition(pointLight.position);
        renderScene(lampModel,lampShader,ourModel, ourShader, lightCube1, coord_system,pointLight.position,glm::vec4(0.0,-1.0,0.0,water_height));
        water_FBO.unbindCurrentBuffer();

        lightCube1.setLightPosition(pointLight.position);
        renderScene(lampModel,lampShader,ourModel, ourShader, lightCube1, coord_system,pointLight.position,glm::vec4(0.0,0.0,0.0,0.0));

        if(programState->showSmallMaps){
            smallReflectionMap.draw(smallMapShader,water_FBO.getReflectionTexture());
            smallRefractionMap.draw(smallMapShader,water_FBO.getRefractionTexture());
        }

        terrain_water.draw(waterShader,programState->camera,SCR_WIDTH,SCR_HEIGHT,water_FBO, glm::vec3(programState->lightPos3f[0], programState->lightPos3f[1], programState->lightPos3f[2]),pointLight.position);

//        if (programState->ImGuiEnabled) {
            DrawImGui(programState);
//        }
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

//    programState->SaveToFile("resources/program_state.txt");
    delete programState;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        programState->camera.ProcessKeyboard(UP, deltaTime);
}
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (programState->CameraMouseMovementUpdateEnabled)
        programState->camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    programState->camera.ProcessMouseScroll(yoffset);
}

void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("Controls");{}
        ImGui::Text("WASD to move, Q/E Down/Up, F1 Options");
        ImGui::End();
    }
    if(programState->ImGuiEnabled){
        {

            ImGui::Begin("Options");
            ImGui::SliderFloat("Camera speed", &programState->camera.MovementSpeed, 15.0, 30.0);
            ImGui::ColorEdit3("Background color", (float *) &programState->clearColor);
            ImGui::Checkbox("Reflection and refraction maps", &programState->showSmallMaps);
            ImGui::Checkbox("Wireframe draw", &programState->wireFrameOption);
            ImGui::SliderFloat3("lamp1 light pos",&programState->lightPos3f[0],0.0f,100.0f);
//            cout<< programState->lightPos3f[0] << " " << programState->lightPos3f[1] << " " << programState->lightPos3f[2] << " " <<std::endl;
//        ImGui::DragFloat3("Backpack position", (float*)&programState->backpackPosition);
//        ImGui::DragFloat("Backpack scale", &programState->backpackScale, 0.05, 0.1, 4.0);


            ImGui::DragFloat("pointLight.constant", &programState->pointLight.constant, 0.05, 0.0, 1.0);
            ImGui::DragFloat("pointLight.linear", &programState->pointLight.linear, 0.05, 0.0, 1.0);
            ImGui::DragFloat("pointLight.quadratic", &programState->pointLight.quadratic, 0.05, 0.0, 1.0);
            ImGui::End();
        }

        {
            ImGui::Begin("Camera info");
            const Camera& c = programState->camera;
            ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
            ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
            ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
            ImGui::Checkbox("Camera mouse update", &programState->CameraMouseMovementUpdateEnabled);
            ImGui::End();
        }
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
        if (programState->ImGuiEnabled) {
            programState->CameraMouseMovementUpdateEnabled = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            programState->CameraMouseMovementUpdateEnabled = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
}
