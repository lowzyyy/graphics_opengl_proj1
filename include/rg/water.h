//
// Created by lowzyyy on 9/16/21.
//

#ifndef PROJECT_BASE_WATER_H
#define PROJECT_BASE_WATER_H

class water{
public:
    unsigned int VAO_quad,VBO_quad;
    float mScale;
    water(float scale){
        mScale = scale;
        float quad_vertices[] = { -1, -1, -1, 1, 1, -1,
                                  1, -1, -1, 1,  1, 1 };
        glGenVertexArrays(1, &VAO_quad);
        glGenBuffers(1, &VBO_quad);

        glBindVertexArray(VAO_quad);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_quad);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
    
    void draw(Shader& waterShader,Camera& kamera,const unsigned int SCR_WIDTH,const unsigned int SCR_HEIGHT){
        glm::mat4 model_water = glm::mat4(1.0);
        model_water = glm::scale(model_water,glm::vec3(mScale,1.0,mScale));

        glm::mat4 pogled_water = glm::mat4(1.0);
        pogled_water = kamera.GetViewMatrix();
        glm::mat4 projekcija_water = glm::mat4(1.0);
        projekcija_water = glm::perspective(glm::radians(kamera.Zoom),(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);

        waterShader.use();

        waterShader.setMat4("model",model_water);
        waterShader.setMat4("pogled",pogled_water);
        waterShader.setMat4("projekcija",projekcija_water);

        glBindVertexArray(VAO_quad);
        glDrawArrays(GL_TRIANGLES,0,6);
        glBindVertexArray(0);
    }
};
#endif //PROJECT_BASE_WATER_H
