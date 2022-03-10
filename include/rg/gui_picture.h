//
// Created by lowzyyy on 9/16/21.
//

#ifndef PROJECT_BASE_GUI_PICTURE_H
#define PROJECT_BASE_GUI_PICTURE_H
class gui_picture{
public:
    unsigned int VAO_quad,VBO_quad, EBO;
    float position_x,position_y;
    bool flipCoors = false;
    unsigned int tekstura;
    gui_picture(float x, float y,bool flip): position_x(x),position_y(y), flipCoors(flip){
                                  //position   //tex coord
        float quad_vertices[] = { -1.0, -1.0,  0.0 ,0.0,
                                  -1.0, 1.0,   0.0,1.0,
                                  1.0, -1.0,   1.0,0.0,
                                  1.0, 1.0,    1.0,1.0
        };
        //position   //tex coord
        float quad_vertices_flip[] = { -1.0, -1.0,  0.0 ,1.0,
                                        -1.0, 1.0,   0.0,0.0,
                                        1.0, -1.0,   1.0,1.0,
                                        1.0, 1.0,    1.0,0.0
        };

        unsigned int indices[] = {
                0, 1, 3, // first triangle
                0, 3, 2  // second triangle
        };
        glGenVertexArrays(1, &VAO_quad);
        glGenBuffers(1, &VBO_quad);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO_quad);

        if(flipCoors){
            glBindBuffer(GL_ARRAY_BUFFER, VBO_quad);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices_flip), quad_vertices_flip, GL_STATIC_DRAW);
        }
        else {
            glBindBuffer(GL_ARRAY_BUFFER, VBO_quad);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        // position attribute
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
//        tekstura = load_texture("resources/textures/awesomeface.png",GL_RGBA);
    }
    void draw(Shader& guiShader,unsigned int texture){
        guiShader.use();
        guiShader.setInt("map",0);
        glm::mat4 model = glm::mat4(1.0);
        model = glm::translate(model,glm::vec3(position_x,position_y,0.0));
        model = glm::scale(model,glm::vec3(0.4,0.3,1.0));
        guiShader.setMat4("model",model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture);
        glBindVertexArray(VAO_quad);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,0);
        glBindTexture(GL_TEXTURE_2D,0);


    }
    unsigned int load_texture(std::string pathOfTexture, GLenum format){
        unsigned int tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
#endif //PROJECT_BASE_GUI_PICTURE_H
