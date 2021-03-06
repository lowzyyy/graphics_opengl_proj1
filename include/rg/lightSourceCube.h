//
// Created by lowzyyy on 9/15/21.
//

#ifndef PROJECT_BASE_LIGHTSOURCECUBE_H
#define PROJECT_BASE_LIGHTSOURCECUBE_H

class lightSourceCube{
private:
    static unsigned VAO_cube;
    static unsigned int VBO_cube;
public:
    glm::vec4 mPlane;
    Shader lightSourceShader;
    glm::vec3 lightPosition;
    float rotationAngle = 0;

    void setRotationAngle(const float rotation_Angle){
        rotationAngle = rotation_Angle;
    }
    void setLightPosition(const glm::vec3 &lightPos) {
        lightSourceCube::lightPosition = lightPos;
    }
    static unsigned int getVAO(){
        return VAO_cube;
    }
    static void Init(){
        float cube_vertices[] = {
                // positions                        // normals                   // texture coords
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
                0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
                0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
                0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
                -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

                -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
                0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
                -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
                -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

                -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
                -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
                -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
                -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

                0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
                0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
                0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
                0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
                0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

                -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
                0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
                0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
                0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
                -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

                -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
                0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
                -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
                -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
        };
        glGenVertexArrays(1, &VAO_cube);
        glGenBuffers(1, &VBO_cube);

        glBindVertexArray(VAO_cube);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        //normals attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        //texture coordinates
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
    lightSourceCube(Shader& shader, const glm::vec4& plane = glm::vec4(0.0,0.0,0.0,0.0)): lightSourceShader(shader), mPlane(plane){

    }

    void draw(Camera& kamera,const unsigned int SCR_WIDTH,const unsigned int SCR_HEIGHT,glm::vec3 lightSourceColor = glm::vec3(4.0)){
        glm::mat4 model_lightCube = glm::mat4(1.0);
        model_lightCube = glm::translate(model_lightCube,lightPosition);
        model_lightCube = glm::rotate(model_lightCube,glm::radians(rotationAngle),glm::vec3(0.0,1.0,0.0));
//        std::cout << lightPosition.x << " "<< lightPosition.y << " " << lightPosition.z << std::endl;

        glm::mat4 pogled_lightCube = glm::mat4(1.0);
        pogled_lightCube = kamera.GetViewMatrix();
        glm::mat4 projekcija_lightCube = glm::mat4(1.0);
        projekcija_lightCube = glm::perspective(glm::radians(kamera.Zoom),(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);

        lightSourceShader.use();
        lightSourceShader.setMat4("model",model_lightCube);
        lightSourceShader.setMat4("pogled",pogled_lightCube);
        lightSourceShader.setMat4("projekcija",projekcija_lightCube);
        lightSourceShader.setVec4("plane",mPlane);

//        glm::vec3 lightSourceColor = glm::vec3(1.0);
        lightSourceShader.setVec3("lightSourceColor",lightSourceColor);

        glBindVertexArray(VAO_cube);
        glDrawArrays(GL_TRIANGLES,0,36);
        glBindVertexArray(0);
    }
};
#endif //PROJECT_BASE_LIGHTSOURCECUBE_H
