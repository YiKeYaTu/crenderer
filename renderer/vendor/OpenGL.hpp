//
// Created by lc on 2022/4/8.
//

#ifndef CRD_OPENGL_HPP
#define CRD_OPENGL_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace OpenGL {
    enum OpenGLBufferType {
        FRAME_BUFFER,
        RENDER_BUFFER,
        TEXTURE_2D_BUFFER
    };
    struct OpenGLBuffer {
        unsigned int data = -1;
        OpenGLBufferType type;
        OpenGLBuffer(unsigned int data, OpenGLBufferType type): data(data), type(type) {}
    };

    struct OpenGLFrameBuffer: public OpenGLBuffer {
        OpenGLFrameBuffer(unsigned int data = -1): OpenGLBuffer(data, OpenGLBufferType::FRAME_BUFFER) {}
    };

    struct OpenGLRenderBuffer: public OpenGLBuffer {
        OpenGLRenderBuffer(unsigned int data = -1): OpenGLBuffer(data, OpenGLBufferType::RENDER_BUFFER) {}
    };

    struct OpenGLTextureBuffer: public OpenGLBuffer {
        OpenGLTextureBuffer(unsigned int data = -1): OpenGLBuffer(data, OpenGLBufferType::TEXTURE_2D_BUFFER) {}
    };

    namespace Window {
        void setViewPort(unsigned int width, unsigned int height) {
            glViewport(0, 0, width, height);
        }
    }

    void init() {
        glfwInit();
        glfwWindowHint(GLFW_SAMPLES, 8);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    }

    void enable(unsigned int flags) {
        glEnable(flags);
    }

    void clearColor(float R = 0.1, float G = 0.1, float B = 0.1) {
        glClearColor(R, G, B, 1.f);
    }

    void clear(unsigned int flags) {
        glClear(flags);
    }

    void use(const OpenGLBuffer& openGLBuffer) {
        switch (openGLBuffer.type) {
            case FRAME_BUFFER:
                glBindFramebuffer(GL_FRAMEBUFFER, openGLBuffer.data);
                return;
            case RENDER_BUFFER:
                glBindRenderbuffer(GL_RENDERBUFFER, openGLBuffer.data);
                return;
            case TEXTURE_2D_BUFFER:
                glBindTexture(GL_TEXTURE_2D, openGLBuffer.data);
                break;
        }
    }

    void useDefault(const OpenGLBuffer& openGLBuffer) {
        OpenGLBuffer defaultBuffer(0, openGLBuffer.type);
        use(defaultBuffer);
    }

    OpenGLRenderBuffer createRenderBuffer(unsigned int width, unsigned int height) {
        unsigned int RBO;
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        return OpenGLRenderBuffer(RBO);
    }

    OpenGLFrameBuffer createFrameBuffer(const OpenGLTextureBuffer& openGLTextureBuffer, const OpenGLRenderBuffer& openGLRenderBuffer = OpenGLRenderBuffer(-1)) {
        unsigned int FBO;
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGLTextureBuffer.data, 0);

        if (openGLRenderBuffer.data > 0) {
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, openGLRenderBuffer.data);
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            throw std::runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return OpenGLFrameBuffer(FBO);
    }

    OpenGLTextureBuffer createRGBTexture2D(unsigned int width, unsigned int height, const char* data = NULL) {
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        return OpenGLTextureBuffer(texture);
    }

    OpenGLTextureBuffer createFloat32RedTexture2D(unsigned int width, unsigned int height, const char* data = NULL) {
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D, 0);
        return OpenGLTextureBuffer(texture);
    }
};

#endif //CRD_OPENGL_HPP
