    //
// Created by lowzyyy on 9/16/21.
//

#ifndef PROJECT_BASE_WATERFRAMEBUFFERS_H
#define PROJECT_BASE_WATERFRAMEBUFFERS_H
class waterFrameBuffers{
    int mdisplayWidth, mdisplayHeight;
private:
    int REFLECTION_WIDTH = 1025;
    int REFLECTION_HEIGHT = 768;
	
    int REFRACTION_WIDTH = 640;
    int REFRACTION_HEIGHT = 480;

    unsigned int reflectionFrameBuffer;
    unsigned int reflectionTexture;
    unsigned int reflectionDepthBuffer;

    unsigned int refractionFrameBuffer;
    unsigned int refractionTexture;
    unsigned int refractionDepthTexture;
public:
    waterFrameBuffers(int displayWidth, int displayHeight){
        mdisplayWidth= displayWidth;
        mdisplayHeight = displayHeight;

        initialiseReflectionFrameBuffer();
        initialiseRefractionFrameBuffer();
    }
    unsigned int getReflectionTexture(){
        return reflectionTexture;
    }
    unsigned int getRefractionTexture(){
        return refractionTexture;
    }
    void unbindCurrentBuffer(){
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        glViewport(0,0,mdisplayWidth,mdisplayHeight);
    }
    void bindReflectionFrameBuffer(){
        bindFrameBuffer(reflectionFrameBuffer,REFLECTION_WIDTH,REFLECTION_HEIGHT);
    }
    void bindRefractionFrameBuffer(){
        bindFrameBuffer(refractionFrameBuffer,REFRACTION_WIDTH,REFRACTION_HEIGHT);
    }
    void bindFrameBuffer(unsigned int FBO,int width,int height){
        glBindTexture(GL_TEXTURE_2D,0);
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0,0,width,height);

    }
    void initialiseReflectionFrameBuffer() {
        createFrameBuffer(1,reflectionFrameBuffer);
        //reflection texture attachment
        createTextureAttachment(1,reflectionTexture,REFLECTION_WIDTH,REFLECTION_HEIGHT);
        //reflection depth buffer
        glGenRenderbuffers(1,&reflectionDepthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER,reflectionDepthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,REFLECTION_WIDTH,REFLECTION_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,reflectionDepthBuffer);
        unbindCurrentBuffer();
    }
    void initialiseRefractionFrameBuffer() {
        createFrameBuffer(1,refractionFrameBuffer);
        //refraction texture attachment
        createTextureAttachment(1,refractionTexture,REFRACTION_WIDTH,REFRACTION_HEIGHT);
        //depth texture attachment
        glGenTextures(1, &refractionDepthTexture);
        glBindTexture(GL_TEXTURE_2D, refractionDepthTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, REFRACTION_WIDTH, REFRACTION_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, refractionDepthTexture, 0);

          //moze i sa ovim metodom kao kod reflection. Ne znam koja je razlika (edit: render bufferi su brzi od tekstura ali se isto moze postici valjda)
//        glGenRenderbuffers(1,&refractionDepthTexture);
//        glBindRenderbuffer(GL_RENDERBUFFER,refractionDepthTexture);
//        glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,REFRACTION_WIDTH,REFLECTION_HEIGHT);
//        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,refractionDepthTexture);

        unbindCurrentBuffer();
    }
    void createFrameBuffer(int numOfBuffers,unsigned int& FBO) {
        glGenFramebuffers(numOfBuffers,&FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    }
    void createTextureAttachment(int numOfTextures,unsigned int& texture,int width,int height){
        glGenTextures(numOfTextures, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    }

};
#endif //PROJECT_BASE_WATERFRAMEBUFFERS_H
