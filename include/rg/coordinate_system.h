//
// Created by lowzyyy on 9/14/21.
//

#ifndef PROJECT_BASE_COORDINATE_SYSTEM_H
#define PROJECT_BASE_COORDINATE_SYSTEM_H
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
class coordinate_system {
public:
    unsigned int VBO_coordiante_system, VAO_coordiante_system;
    coordinate_system(){

        float coordinate_sytem_vertices[]={
                //coordinates       //color
                -5.0,0.0,0.0,       1.0,0.0,0.0,    //x axis
               100.0,0.0,0.0,       1.0,0.0,0.0,
                0.0,-5.0,0.0,       0.0,1.0,0.0,    //y axis
                0.0,100.0,0.0,       0.0,1.0,0.0,
                0.0,0.0,-5.0,       0.0,0.0,1.0,    //z axis
                0.0,0.0,100.0,       0.0,0.0,1.0
        };


        glGenBuffers(1, &VBO_coordiante_system);
        glGenVertexArrays(1, &VAO_coordiante_system);

        glBindVertexArray(VAO_coordiante_system);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_coordiante_system);
        glBufferData(GL_ARRAY_BUFFER, sizeof(coordinate_sytem_vertices), coordinate_sytem_vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

    void draw(Shader& coordinateSystemShader,Camera& kamera,const unsigned int SCR_WIDTH,const unsigned int SCR_HEIGHT){
        glm::mat4 model_coordinate_system = glm::mat4(1.0);
        glm::mat4 pogled_coordinate_system = glm::mat4(1.0);
        pogled_coordinate_system = kamera.GetViewMatrix();
        glm::mat4 projekcija_coordinate_system = glm::mat4(1.0);
        projekcija_coordinate_system = glm::perspective(glm::radians(kamera.Zoom),(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);

        coordinateSystemShader.use();

        coordinateSystemShader.setMat4("model",model_coordinate_system);
        coordinateSystemShader.setMat4("pogled",pogled_coordinate_system);
        coordinateSystemShader.setMat4("projekcija",projekcija_coordinate_system);
        glBindVertexArray(VAO_coordiante_system);
        glDrawArrays(GL_LINES, 0,6);
    }
};

#endif //PROJECT_BASE_COORDINATE_SYSTEM_H
