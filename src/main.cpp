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
#include <random>
#include <list>

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
    glm::vec3 clearColor = glm::vec3(165.0/255,138.0/255,138.0/255);
    bool ImGuiEnabled = false;
    bool showSmallMaps = false;
    bool wireFrameOption = false;
    bool waterSpecular = false;
    float lightPos3f[3];
    float distortionWater = 0.02;
    float shininessWater = 125;
    Camera camera;
    bool CameraMouseMovementUpdateEnabled = true;
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
void renderScene(list<std::pair<Model,Shader>> &modeli, lightSourceCube &lightCube1, coordinate_system& coord_system, glm::vec4 clipPlane);
void setShaderLights(Shader &shader, std::vector<PointLight*> &lights,int);
void setMatrixAndLightsIsland(Shader &shader, vector<PointLight *> pointLights,int numberOfPointLights);


void setMatrixAndLightsAll(Shader shader, glm::mat4 &model, vector<PointLight *> pointLights, int numberOfPointLights);

void setMatrixAndLightsInstanced(Shader shader, glm::mat4 *modelsInstanced, vector<PointLight *> pointLights,
                                 int numberOfPointLights);

void createModelsInstancedFish(glm::mat4 *models, int fishPerLine,float ground_zero_height, float water_height);

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
//    stbi_set_flip_vertically_on_load(true);

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
    //center the window on the screen
    CenterTheWindow(window,SCR_WIDTH,SCR_HEIGHT);
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CLIP_DISTANCE0);

    //build and compile SHADERS BEGIN-------------------------------------------------
    Shader ourShader("resources/shaders/modelVertexShader.vs", "resources/shaders/modelFragmentShader.fs");
    Shader lampShader("resources/shaders/modelVertexShader.vs", "resources/shaders/modelFragmentShader.fs");
    Shader fishShader("resources/shaders/fishModelInstancedVertexShader.vs", "resources/shaders/modelFragmentShader.fs");
    Shader coordinateSystemShader("resources/shaders/coordinateSystemVertexShader.vs","resources/shaders/coordinateSystemFragmentShader.fs");
    Shader lightSourceShader("resources/shaders/lightSourceVertexShader.vs", "resources/shaders/lightSourceFragmentShader.fs");
    Shader waterShader("resources/shaders/waterVertexShader.vs","resources/shaders/waterFragmentShader.fs");
    Shader smallMapShader("resources/shaders/smallMapVertexShader.vs","resources/shaders/smallMapFragmentShader.fs");
    //build and compile SHADERS END-------------------------------------------------

    // load models BEGIN ------------------------------------------------------------------
    Model ourModel("resources/objects/ostrvo2/Small Tropical Island/Small Tropical Island_no_plane.obj");
    Model lampModel("resources/objects/lampa1/Street Lamp/StreetLamp.obj");
    Model fishModel("resources/objects/fish1/ForPetar/fish_final.obj");
    ourModel.SetShaderTextureNamePrefix("material.");
    lampModel.SetShaderTextureNamePrefix("material.");
    fishModel.SetShaderTextureNamePrefix("material");
    // load models END------------------------------------------------------------------

    //store all models in a list
    std::list<std::pair<Model,Shader>> modeli;
    modeli.insert(modeli.end(),make_pair(ourModel,ourShader));
    modeli.insert(modeli.end(),make_pair(lampModel,lampShader));
    //all pointlights
    vector<PointLight*> pointLights;

    //rotating lightcube begin---------------------------------------------------------------------------------
    PointLight& pointLightCube = programState->pointLight;
    pointLightCube.position = glm::vec3(4.0f, 5, 0.0);
    pointLightCube.ambient = glm::vec3(0.1, 0.1, 0.1);
    pointLightCube.diffuse = glm::vec3(0.6, 0.6, 0.6);
    pointLightCube.specular = glm::vec3(1.0, 1.0, 1.0);

    pointLightCube.constant = 1.0f;
    pointLightCube.linear = 0.09f;
    pointLightCube.quadratic = 0.032f;
    pointLights.push_back(&pointLightCube);
    //rotating lightcube end---------------------------------------------------------------------------------

    //lamp light begin---------------------------------------------------------------------------------
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
    pointLights.push_back(&pointLight_lamp);
    //lamp light end---------------------------------------------------------------------------------

    //define additional objects and water begin --------------------------------------------------
    float water_height = 1;
    float ground_zero = 1.5;
    glm::vec4 waterClipPlane= glm::vec4(0.0,1,0.0,water_height);
    coordinate_system coord_system(coordinateSystemShader);
    lightSourceCube lightCube1(lightSourceShader);
    water terrain_water(200.0,water_height,waterClipPlane);
    waterFrameBuffers water_FBO(SCR_WIDTH,SCR_HEIGHT);
    //fish
    int fishPerLine = 1;
    int fish_amount = fishPerLine * fishPerLine;
    glm::mat4* modelsInstanced;
    modelsInstanced = new glm::mat4[fish_amount];
    //TODO napraviti funkciju za kreiranje model matrica createModelsInstancedFish()
    createModelsInstancedFish(modelsInstanced,fishPerLine,ground_zero,water_height);
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, fish_amount * sizeof(glm::mat4), &modelsInstanced[0], GL_STATIC_DRAW);
    for (unsigned int i = 0; i < fishModel.meshes.size(); i++)
    {
        unsigned int VAO = fishModel.meshes[i].VAO;
        glBindVertexArray(VAO);
        // set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    gui_picture smallReflectionMap(0.5,0.5,true);
    gui_picture smallRefractionMap(0.5,-0.5,false);

    //define additional objects and water end --------------------------------------------------

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

        int numberOfPointlights = pointLights.size();
        //update light position every frame
        pointLightCube.position = glm::vec3(10.0 * cos(currentFrame), 25.0f, 10.0 * sin(currentFrame));
        pointLight_lamp.position = glm::vec3(programState->lightPos3f[0],programState->lightPos3f[1],programState->lightPos3f[2]);
//        ---------------------------------ISLAND-------------------------------
        ourShader.use();
        setMatrixAndLightsIsland(ourShader,pointLights,numberOfPointlights);
        ourShader.setVec3("viewPosition", programState->camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);

//        ---------------------------LAMP---------------------------

        lampShader.use();
        glm::mat4 model_lamp = glm::mat4(1.0);
        model_lamp = glm::translate(model_lamp,glm::vec3(100.0,-1.0,100.0));
        model_lamp = glm::rotate(model_lamp, glm::radians(135.0f),glm::vec3(0.0,1.0,0.0));

        setMatrixAndLightsAll(lampShader,model_lamp,pointLights,numberOfPointlights);

        lampShader.setVec3("viewPosition", programState->camera.Position);
        lampShader.setFloat("material.shininess", 32.0f);
//        ------------------------------FISH----------------------------

        fishShader.use();
        setMatrixAndLightsInstanced(fishShader, modelsInstanced, pointLights,numberOfPointlights);
        fishShader.setInt("texture_diffuse1", 0);
        fishShader.setInt("texture_normal1", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fishModel.textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fishModel.textures_loaded[1].id);
        for (unsigned int i = 0; i < fishModel.meshes.size(); i++)
        {
            glBindVertexArray(fishModel.meshes[i].VAO);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(fishModel.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, fish_amount);
            glDisable(GL_CULL_FACE);
            glBindVertexArray(0);
        }

        fishShader.setVec3("viewPosition", programState->camera.Position);
        fishShader.setFloat("material.shininess", 32.0f);
        fishShader.setVec4("plane",waterClipPlane);


//      ------------------------------------ RENDER -------------------------------------------
        //reflection map render
        water_FBO.bindReflectionFrameBuffer();
        programState->camera.invertPitch();
        programState->camera.invertY();
        glm::mat4 view = programState->camera.GetViewMatrix();
        ourShader.use();
        ourShader.setMat4("view", view);
        lampShader.use();
        lampShader.setMat4("view", view);
        renderScene(modeli, lightCube1, coord_system,glm::vec4(0.0,1.0,0.0,-water_height));
        programState->camera.invertPitch();
        programState->camera.invertY();
        water_FBO.unbindCurrentBuffer();

        //refraction map render
        water_FBO.bindRefractionFrameBuffer();
        view = programState->camera.GetViewMatrix();
        ourShader.use();
        ourShader.setMat4("view", view);
        lampShader.use();
        lampShader.setMat4("view", view);
        renderScene(modeli, lightCube1, coord_system,glm::vec4(0.0,-1.0,0.0,water_height));
        water_FBO.unbindCurrentBuffer();

        //render the scene finally
        //if above water, clip everything under it, else clip everything under ground zero
        if(programState->camera.Position.y > water_height)
            waterClipPlane.w = water_height;
        else
            waterClipPlane.w = ground_zero;

        renderScene(modeli, lightCube1, coord_system,waterClipPlane);

        if(programState->showSmallMaps){
            smallReflectionMap.draw(smallMapShader,water_FBO.getReflectionTexture());
            smallRefractionMap.draw(smallMapShader,water_FBO.getRefractionTexture());
        }

        terrain_water.setDistortionStrentgh(programState->distortionWater);
        if(programState->waterSpecular)
            terrain_water.specularWater = true;
        else
            terrain_water.specularWater = false;
        terrain_water.setShininess(programState->shininessWater);
        waterShader.use();
        setShaderLights(waterShader,pointLights,numberOfPointlights);
        terrain_water.draw(waterShader,programState->camera,SCR_WIDTH,SCR_HEIGHT,water_FBO);



        DrawImGui(programState);
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
//function definitons
void createModelsInstancedFish(glm::mat4 *models, int fishPerLine, float ground_zero_height, float water_height) {
    int step = 400 / fishPerLine;
    int fish_amount = fishPerLine * fishPerLine;
    float step_offset = step/5.0;
    //pick position in every step from current step to next step in offset (not to pick edges of a spawn zone of a fish)
    int next_step_x = 200 - step;
    int next_step_z = 200 - step;
    float x,z;
    float y = -ground_zero_height + (ground_zero_height-water_height)/0.33; //closer to ground_zero then to water surface
    srand(static_cast<unsigned int>(glfwGetTime()));
    int fishCounter = 0;
    std::random_device rd;
    std::mt19937 mt(rd());
    int fishCounter_z = 0;
    int fishCounter_x = 0;
    for(int i=200;fishCounter_x++<fishPerLine;i-=step){
        next_step_z = 200 - step;
        fishCounter_z = 0;
        for(int j=200;fishCounter_z++<fishPerLine;j-=step){

            std::uniform_real_distribution<double> dist_x(next_step_x + step_offset, i-step_offset);
            x = dist_x(mt);
            std::uniform_real_distribution<double> dist_z(next_step_z + step_offset, j-step_offset);
            z = dist_z(mt);
            cout<< x<< " " << z <<endl;
//            x = fmodf(rand(),(i-step_offset)) + (next_step_x + step_offset);  //pick x from range (i-offset, next_step-offset)
//            z = fmodf(rand(),(j-step_offset)) + (next_step_z + step_offset);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(x, 10, z));
            model = glm::scale(model,glm::vec3(0.05));
            models[fishCounter++] = model;
            next_step_z -= step;
        }
        next_step_x -= step;
    }
}

void setMatrixAndLightsInstanced(Shader shader, glm::mat4 *modelsInstanced, vector<PointLight *> pointLights, int numberOfPointLights) {
    setShaderLights(shader,pointLights,numberOfPointLights);
    glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),(float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 500.0f);
    glm::mat4 view= programState->camera.GetViewMatrix();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
}

void setMatrixAndLightsAll(Shader shader, glm::mat4 &model, vector<PointLight *> pointLights, int numberOfPointLights) {
    setShaderLights(shader,pointLights,numberOfPointLights);

    glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),(float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 500.0f);
    glm::mat4 view = programState->camera.GetViewMatrix();

    shader.setMat4("model", model);
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
}

void setMatrixAndLightsIsland(Shader &shader, vector<PointLight *> pointLights,int numberOfPointLights) {
    setShaderLights(shader,pointLights,numberOfPointLights);
    //ISLAND MATRIX
    glm::mat4 projection = glm::perspective(glm::radians(programState->camera.Zoom),(float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 500.0f);
    glm::mat4 view = programState->camera.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0,-2,0.0)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.5));    // it's a bit too big for our scene, so scale it down
    shader.setMat4("model", model);
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
}

void renderScene(list<std::pair<Model,Shader>> &modeli, lightSourceCube &lightCube1, coordinate_system& coord_system, glm::vec4 clipPlane) {
    for(auto& model: modeli){
        model.second.use();
        model.second.setVec4("plane",clipPlane);
        model.first.Draw(model.second);
    }
    lightCube1.mPlane = clipPlane;
    lightCube1.setLightPosition(programState->pointLight.position);
    lightCube1.draw(programState->camera,SCR_WIDTH,SCR_HEIGHT);
    coord_system.mPlane = clipPlane;
    coord_system.draw(programState->camera,SCR_WIDTH,SCR_HEIGHT);
}

void setShaderLights(Shader &shader, std::vector<PointLight*> &lights,int n_lights) {
    shader.setInt("numberOfPointLights",n_lights);
    for(int i=0;i<n_lights;i++){
        shader.setVec3("pointLights["+to_string(i)+"].position", lights[i]->position);
        shader.setVec3("pointLights["+to_string(i)+"].ambient", lights[i]->ambient);
        shader.setVec3("pointLights["+to_string(i)+"].diffuse", lights[i]->diffuse);
        shader.setVec3("pointLights["+to_string(i)+"].specular", lights[i]->specular);
        shader.setFloat("pointLights["+to_string(i)+"].constant", lights[i]->constant);
        shader.setFloat("pointLights["+to_string(i)+"].linear", lights[i]->linear);
        shader.setFloat("pointLights["+to_string(i)+"].quadratic", lights[i]->quadratic);
        }
}

void DrawImGui(ProgramState *programState) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    //always show this menu
    {
        ImGui::Begin("Controls");{}
        ImGui::Text("WASD to move, Q/E Down/Up, F1 Options");
        ImGui::End();
    }
    //additional options if F1 is pressed
    if(programState->ImGuiEnabled){
        {

            ImGui::Begin("Options");
            ImGui::SliderFloat("Camera speed", &programState->camera.MovementSpeed, 20.0, 30.0);
            ImGui::SliderFloat("distortion strength", &programState->distortionWater, 0.01, 0.05);
            ImGui::ColorEdit3("Background color", (float *) &programState->clearColor);
            ImGui::Checkbox("Specular water", &programState->waterSpecular);
            ImGui::SliderFloat("Shininess water", &programState->shininessWater, 2, 256);
            ImGui::Checkbox("Reflection and refraction maps", &programState->showSmallMaps);
            ImGui::Checkbox("Wireframe draw", &programState->wireFrameOption);
            ImGui::SliderFloat3("lamp1 light pos",&programState->lightPos3f[0],0.0f,100.0f);
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
