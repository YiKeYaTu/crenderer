//
// Created by lc on 2022/4/1.
//

#ifndef CRENDERER_GLSLSHADER_HPP
#define CRENDERER_GLSLSHADER_HPP

#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GLSLShader {
private:
    unsigned int _shaderProgram;
    unsigned int _vertexShader, _fragmentShader;
    const std::string _vertexShaderSource, _fragmentShaderSource;

    const std::string _loadShaderSource(const std::string& shaderSourcePath) {
        std::ifstream ifs(shaderSourcePath);
        if (!ifs.is_open()) {
            throw std::runtime_error(std::string("Can't open shader source: ") + shaderSourcePath);
        }
        std::stringstream ss;
        ss << ifs.rdbuf();

        return ss.str();
    }

    void _compileShader(unsigned int shader, const std::string source) {
        const char* p = source.c_str();

        glShaderSource(shader, 1, &p, NULL);
        glCompileShader(shader);

        int  success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            throw std::runtime_error(std::string("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n") + infoLog);
        }
        glAttachShader(_shaderProgram, shader);
    }

    void _linkProgram() {
        glLinkProgram(_shaderProgram);

        int success;
        char infoLog[512];

        glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(_shaderProgram, 512, NULL, infoLog);
            throw std::runtime_error(std::string("ERROR::SHADER::PROGRAM::LINKING_FAILED\n") + infoLog);
        }
    }
public:
    GLSLShader(const std::string& vertexShaderSourcePath, const std::string& fragmentShaderSourcePath)
        : _shaderProgram(glCreateProgram()),
          _vertexShader(glCreateShader(GL_VERTEX_SHADER)), _fragmentShader(glCreateShader(GL_FRAGMENT_SHADER)),
          _vertexShaderSource(_loadShaderSource(vertexShaderSourcePath)),
          _fragmentShaderSource(_loadShaderSource(fragmentShaderSourcePath)) {

        _compileShader(_vertexShader, _vertexShaderSource);
        _compileShader(_fragmentShader, _fragmentShaderSource);

        _linkProgram();

        glDeleteShader(_vertexShader);
        glDeleteShader(_fragmentShader);
    }

    void use() {
        glUseProgram(_shaderProgram);
    }

    void setUniform(const std::string name, const Vec4f& vec4f) {
        glUseProgram(_shaderProgram);
        glUniform4fv(glGetUniformLocation(_shaderProgram, name.c_str()), 1, vec4f.data());
    }

    void setUniform(const std::string name, const Vec3f& vec3f) {
        glUseProgram(_shaderProgram);
        glUniform3fv(glGetUniformLocation(_shaderProgram, name.c_str()), 1, vec3f.data());
    }

    void setUniform(const std::string name, const Mat4f& mat4f) {
        glUseProgram(_shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, name.c_str()), 1, GL_FALSE, mat4f.data());
    }
};

#endif //CRENDERER_GLSLSHADER_HPP
