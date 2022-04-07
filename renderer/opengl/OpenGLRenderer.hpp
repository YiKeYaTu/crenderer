//
// Created by lc on 2022/3/31.
//

#ifndef CRENDERER_OPENGLRENDERER_HPP
#define CRENDERER_OPENGLRENDERER_HPP

#include <renderer/Renderer.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader/ShaderGLSL.hpp>
#include <scene/Camera.hpp>
#include <object/primitive/Triangle.hpp>
#include <unordered_map>
#include <material/MaterialOpenGL.hpp>

class OpenGLRenderer: public Renderer {
private:
    std::string _windowTitle;

    static void _framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }
    static void _initOpenGL() {
        static bool inited = false;
        if (inited) {
            return;
        }

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    }

    GLFWwindow* _createWindow(const Scene& scene) const {
        GLFWwindow* window = glfwCreateWindow(
                scene.width(),
                scene.height(),
                _windowTitle.c_str(),
                NULL,
                NULL
        );

        if (window == NULL) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, _framebufferSizeCallback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw std::runtime_error("Failed to initialize GLAD");
        }

        return window;
    }

    unsigned int _processInput(GLFWwindow *window, const Camera& camera) const {
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, true);
        } else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W)) {
            camera.moveForward(0.1);
        } else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S)) {
            camera.moveForward(-0.1);
        } else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A)) {
            camera.moveSide(-0.1);
        } else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D)) {
            camera.moveSide(0.1);
        } else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT)) {
            camera.rotateRaw(-3.0 / 180 * M_PI);
        } else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT)) {
            camera.rotateRaw(3.0 / 180 * M_PI);
        } else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP)) {
            camera.rotatePitch(-3.0 / 180 * M_PI);
        } else if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN)) {
            camera.rotatePitch(3.0 / 180 * M_PI);
        }
    }

public:
    ~OpenGLRenderer() {
        glfwTerminate();
    }

    static OpenGLRenderer createOpenGLRenderer() {
        _initOpenGL();
        return OpenGLRenderer();
    }

    void render(
        const Scene& scene,
        const Camera& camera,
        std::function<void (
            std::unordered_map<std::string, MaterialOpenGL>& materials
        )> initFunc,
        std::function<void (
            const Mat4f& view,
            const Mat4f& projection,
            std::unordered_map<std::string, MaterialOpenGL>& materials
        )> renderingFunc,
        std::function<void (
            std::unordered_map<std::string, MaterialOpenGL>& materials
        )> uninitFunc
    ) const {
        const auto& materials = scene.materials();
        std::unordered_map<std::string, MaterialOpenGL> openGLMaterials;

        for (const auto& material : materials) {
            openGLMaterials.emplace(material.first, material.second);
        }

        GLFWwindow* window = _createWindow(scene);
        initFunc(openGLMaterials);

        Mat4f projectionMatrix(camera.calcProjectionMatrix(static_cast<float>(scene.width()) / scene.height()));

        while (!glfwWindowShouldClose(window)) {
            _processInput(window, camera);

            renderingFunc(camera.calcViewMatrix(), projectionMatrix, openGLMaterials);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        uninitFunc(openGLMaterials);
    }
};

#endif //CRENDERER_OPENGLRENDERER_HPP
