//
// Created by lowzyyy on 9/16/21.
//

#ifndef PROJECT_BASE_WATER_H
#define PROJECT_BASE_WATER_H
#include <rg/waterFrameBuffers.h>
class water{
public:
    unsigned int VAO_quad,VBO_quad;
    float mScale,mHeight;
    glm::vec4 mPlane;
    unsigned int dudv_texture;
    unsigned int normal_texture;
    float mDist = 0.02;
    float mShininess = 125;
    bool specularWater;
    water(float scale,float height,glm::vec4& plane) :mScale(scale),mHeight(height),mPlane(plane){
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

        glBindVertexArray(0);
        dudv_texture = load_texture("resources/textures/dudv_map1.png",GL_RGB);
        normal_texture = load_texture("resources/textures/normal_map1.png",GL_RGB);
    }
    
    void draw(Shader& waterShader,Camera& kamera,const unsigned int SCR_WIDTH,const unsigned int SCR_HEIGHT,waterFrameBuffers& water_fbo){
        glm::mat4 model_water = glm::mat4(1.0);
        model_water = glm::scale(model_water,glm::vec3(mScale,1.0,mScale));

        glm::mat4 pogled_water = glm::mat4(1.0);
        pogled_water = kamera.GetViewMatrix();
        glm::mat4 projekcija_water = glm::mat4(1.0);
        projekcija_water = glm::perspective(glm::radians(kamera.Zoom),(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);

        waterShader.use();
        if(specularWater)
            waterShader.setInt("specularWater", 1);
        else
            waterShader.setInt("specularWater", 0);

        waterShader.setInt("reflectionTexture", 0);
        waterShader.setInt("refractionTexture",1);
        waterShader.setInt("DuDvMap",2);
        waterShader.setInt("normalMap",3);
        waterShader.setFloat("distortionStrength",mDist);
//        cout<< "dist " << mDist << std::endl;

        waterShader.setVec3("viewPos", kamera.Position);

        waterShader.setVec3("material.specular", 1.0f, 1.0f, 1.0f);
        waterShader.setFloat("material.shininess",mShininess);
        
        waterShader.setMat4("model",model_water);
        waterShader.setMat4("pogled",pogled_water);
        waterShader.setMat4("projekcija",projekcija_water);

        waterShader.setVec4("plane",mPlane);
        waterShader.setFloat("waterHeight",mHeight);
        float moveFactor=0;
        float waveSpeed = 0.05;
        moveFactor += waveSpeed * glfwGetTime();
        moveFactor = fmod(moveFactor,1.0);
        waterShader.setFloat("moveFactor",moveFactor);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,water_fbo.getReflectionTexture());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,water_fbo.getRefractionTexture());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,dudv_texture);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D,normal_texture);
        glBindVertexArray(VAO_quad);
        glDrawArrays(GL_TRIANGLES,0,6);
        glBindVertexArray(0);
    }
    void setDistortionStrentgh(float dist){
        mDist = dist;
    }
    void setShininess(float shin){
        mShininess = shin;
    }
    unsigned int load_texture(std::string pathOfTexture, GLenum format){
        unsigned int tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);


        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
        unsigned char *data = stbi_load(FileSystem::getPath(pathOfTexture).c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        return tex;
    }
};
#endif //PROJECT_BASE_WATER_H
