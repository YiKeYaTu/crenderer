//
// Created by lc on 2022/4/4.
//

#ifndef CRD_MATERIALOPENGL_HPP
#define CRD_MATERIALOPENGL_HPP

#include <material/Material.hpp>
#include <scene/Camera.hpp>
#include <scene/Scene.hpp>
#include <shader/ShaderGLSL.hpp>

#include <loader/TextureLoader.hpp>
#include <loader/Texture2DLoader.hpp>

class MaterialOpenGL: public Material {
    struct BufferUnion {
        unsigned int VBO, VAO, EBO, TEX;
        BufferUnion(unsigned int VBO, unsigned int VAO, unsigned int EBO, unsigned int TEX)
                : VBO(VBO), VAO(VAO), EBO(EBO), TEX(TEX) {}
    };

public:
    MaterialOpenGL(const Material& material)
        : Material(material) {}

    void bind() {
        auto& meshes = Material::_meshLoader->meshes();
        auto& textureLoaders = Material::_textureLoaders;

        for (int i = 0;i < meshes.size(); ++i) {
            const auto& mesh = meshes[i];
            const auto& textureLoader = textureLoaders[textureLoaders.size() == 1 ? 0 : i];

            auto& vertexes = mesh.vertexes();
            auto& indexes = mesh.indexes();

            unsigned int VBO, VAO, EBO;

            glGenBuffers(1, &VBO);
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &EBO);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexes.size(), indexes.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), vertexes.data(), GL_STATIC_DRAW);

            glBindVertexArray(VAO);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));

            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            unsigned int TEX;

            if (textureLoader->textureType() == TextureType::TEXTURE_2D) {
                const Texture2DLoader* texture2DLoader = static_cast<const Texture2DLoader*>(textureLoader);
                glGenTextures(1, &TEX);
                glBindTexture(GL_TEXTURE_2D, TEX);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture2DLoader->width(), texture2DLoader->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, textureLoader->data());
                glGenerateMipmap(GL_TEXTURE_2D);
            }

            _bufferUnions.emplace_back(VBO, VAO, EBO, TEX);
        }
    }

    void unbind() {
        for (const auto& bufferUnion : _bufferUnions) {
            glDeleteBuffers(3, &bufferUnion.VBO);
            glDeleteTextures(1, &bufferUnion.TEX);
        }
    }

    void draw(ShaderGLSL& shader) const {
        for (int i = 0; i < _bufferUnions.size(); ++ i) {
            const BufferUnion& bufferUnion = _bufferUnions[i];

            glBindBuffer(GL_ARRAY_BUFFER, bufferUnion.VBO);
            glBindVertexArray(bufferUnion.VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferUnion.EBO);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, bufferUnion.TEX);

            shader.use();

            glDrawElements(GL_TRIANGLES, _meshLoader->meshes()[i].indexes().size(), GL_UNSIGNED_INT, 0);
        }
    }

//    void drawPerMesh(std::function<void (unsigned int)> fn) const {
//        for (int i = 0; i < _bufferUnions.size(); ++ i) {
//            const BufferUnion& bufferUnion = _bufferUnions[i];
//
//            glBindBuffer(GL_ARRAY_BUFFER, bufferUnion.VBO);
//            glBindVertexArray(bufferUnion.VAO);
//            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferUnion.EBO);
//
//            fn(bufferUnion.TEX);
//
//            glDrawElements(GL_TRIANGLES, _meshLoader->meshes()[i].indexes().size(), GL_UNSIGNED_INT, 0);
//        }
//    }

//    void draw(ShaderGLSL& shader) const {
//        for (int i = 0; i < _bufferUnions.size(); ++ i) {
//            const BufferUnion& bufferUnion = _bufferUnions[i];
//
//            glBindBuffer(GL_ARRAY_BUFFER, bufferUnion.VBO);
//            glBindVertexArray(bufferUnion.VAO);
//            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferUnion.EBO);
//
//            shader.use();
//
//            glDrawElements(GL_TRIANGLES, _meshLoader->meshes()[i].indexes().size(), GL_UNSIGNED_INT, 0);
//        }
//    }
//
//    unsigned int  drawAsTexture(ShaderGLSL& shader) const {}

private:
    std::vector<BufferUnion> _bufferUnions;
};

#endif //CRD_MATERIALOPENGL_HPP
