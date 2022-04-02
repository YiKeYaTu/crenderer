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

class OpenGLRenderer: public Renderer {
private:
    static void _framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    using Renderer::Renderer;

    std::string _windowTitle;

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

    unsigned int _processInput(GLFWwindow *window) const {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        return GLFW_PRESS;
    }

    void _renderMesh(const Mesh<Triangle>& mesh, const Camera& camera) const {
        GLSLShader shader("../glsl/vertexShader.glsl", "../glsl/fragmentShader.glsl");
        shader.setUniform("uProjection", Mat4f::Perspective(0.01, 2000, 90.0 / 180 * M_PI, 1.0));
        shader.setUniform("uModel", Mat4f::Identity());
        shader.setUniform("uView", camera.calcViewMatrix());

        auto& vertexes = mesh.vertexes();
        auto& indexes = mesh.indexes();

        unsigned int VBO, VAO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        if (indexes.size() > 0) {
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned long) * indexes.size(), indexes.data(), GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

        glBindVertexArray(VAO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)3);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)6);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glBindVertexArray(VAO);
        shader.use();
        if (indexes.size() > 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, vertexes.size());
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

    virtual void render(const Scene& scene, const Camera& camera) const override {
        GLFWwindow* window = _createWindow(scene);
        const std::vector<Mesh<Triangle>>& meshes = scene.meshes();

        glEnable(GL_DEPTH_TEST);

        while (!glfwWindowShouldClose(window)) {
            unsigned int pressedKey = _processInput(window);

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (const auto& mesh : meshes) {
                _renderMesh(mesh, camera);
            }

            if (pressedKey == glfwGetKey(window, GLFW_KEY_W)) {
                camera.moveForward(0.01);
            } else if (pressedKey == glfwGetKey(window, GLFW_KEY_S)) {
                camera.moveForward(-0.01);
            } else if (pressedKey == glfwGetKey(window, GLFW_KEY_A)) {
                camera.moveSide(-0.01);
            } else if (pressedKey == glfwGetKey(window, GLFW_KEY_D)) {
                camera.moveSide(0.01);
            } else if (pressedKey == glfwGetKey(window, GLFW_KEY_LEFT)) {
                camera.rotateRaw(-1.0 / 180 * M_PI);
            } else if (pressedKey == glfwGetKey(window, GLFW_KEY_RIGHT)) {
                camera.rotateRaw(1.0 / 180 * M_PI);
            } else if (pressedKey == glfwGetKey(window, GLFW_KEY_UP)) {
                camera.rotatePitch(-1.0 / 180 * M_PI);
            } else if (pressedKey == glfwGetKey(window, GLFW_KEY_DOWN)) {
                camera.rotatePitch(1.0 / 180 * M_PI);
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
};

#endif //CRENDERER_OPENGLRENDERER_HPP
