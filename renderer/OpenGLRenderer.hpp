//
// Created by lc on 2022/3/31.
//

#ifndef CRENDERER_OPENGLRENDERER_HPP
#define CRENDERER_OPENGLRENDERER_HPP

#include <renderer/Renderer.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader/GLSLShader.hpp>
#include <scene/Camera.hpp>
#include <object/primitive/Triangle.hpp>
#include <unordered_map>
#include <material/MaterialOpenGL.hpp>
#include <renderer/vendor/OpenGL.hpp>

class OpenGLRenderer: public Renderer {
private:
    const std::string _windowTitle;
    const Scene& _scene;

    OpenGLRenderer(const std::string& title, const Scene& scene): _windowTitle(title), _scene(scene) {}

    static void _framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        OpenGL::Window::setViewPort(width, height);
    }
    static void _initOpenGL() {
        static bool inited = false;
        if (inited) {
            return;
        }

        OpenGL::init();
    }

    GLFWwindow* _createWindow() const {
        GLFWwindow* window = glfwCreateWindow(
            _scene.width(),
            _scene.height(),
            _windowTitle.c_str(),
            NULL,
            NULL
        );

        if (window == NULL) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
            _framebufferSizeCallback(window, width, height);
        });

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

    static OpenGLRenderer createOpenGLRenderer(const std::string& title, const Scene& scene) {
        _initOpenGL();
        return OpenGLRenderer(title, scene);
    }

    void render(
        const Camera& camera,
        std::function<void (
            std::unordered_map<std::string, MaterialOpenGL>& materials
        )> initFunc,
        std::function<void (
            int width, int height,
            const Mat4f& view,
            const Mat4f& projection,
            std::unordered_map<std::string, MaterialOpenGL>& materials
        )> renderingFunc,
        std::function<void (
            std::unordered_map<std::string, MaterialOpenGL>& materials
        )> uninitFunc
    ) const {
        const auto& materials = _scene.materials();
        std::unordered_map<std::string, MaterialOpenGL> openGLMaterials;

        for (const auto& material : materials) {
            openGLMaterials.emplace(material.first, material.second);
        }

        GLFWwindow* window = _createWindow();
        initFunc(openGLMaterials);

        int width, height;

        while (!glfwWindowShouldClose(window)) {
            _processInput(window, camera);

            glfwGetFramebufferSize(window, &width, &height);
            renderingFunc(width, height, camera.calcViewMatrix(), camera.calcProjectionMatrix(static_cast<float>(width) / height), openGLMaterials);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        uninitFunc(openGLMaterials);
    }
};

#endif //CRENDERER_OPENGLRENDERER_HPP
