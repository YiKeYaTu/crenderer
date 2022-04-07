//
// Created by lc on 2022/4/4.
//

#ifndef CRD_MATERIALOPENGL_HPP
#define CRD_MATERIALOPENGL_HPP

#include <material/Material.hpp>
#include <scene/Camera.hpp>
#include <scene/Scene.hpp>
#include <shader/ShaderGLSL.hpp>

class MaterialOpenGL: public Material {
    struct BufferUnion {
        unsigned int VBO, VAO, EBO;
        BufferUnion(unsigned int VBO, unsigned int VAO, unsigned int EBO)
            : VBO(VBO), VAO(VAO), EBO(EBO) {}
    };
public:
    MaterialOpenGL(const Material& material)
        : Material(material) {}

    virtual ~MaterialOpenGL() {
        unbind();
    }

    void bind() {
        auto& meshes = Material::_loader.meshes();

        for (const auto&mesh : meshes) {
            auto& vertexes = mesh.vertexes();
            auto& indexes = mesh.indexes();

            unsigned int VBO, VAO, EBO;

            glGenBuffers(1, &VBO);
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &EBO);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned long) * indexes.size(), indexes.data(), GL_STATIC_DRAW);

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

            bufferUnions.emplace_back(VBO, VAO, EBO);
        }
    }

    void unbind() {
        for (const auto& bufferUnion : bufferUnions) {
            glDeleteBuffers(3, &bufferUnion.VBO);
        }
    }

    void draw(ShaderGLSL& shader) const {
        for (int i = 0; i < bufferUnions.size(); ++ i) {
            const BufferUnion& bufferUnion = bufferUnions[i];

            glBindBuffer(GL_ARRAY_BUFFER, bufferUnion.VBO);
            glBindVertexArray(bufferUnion.VAO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferUnion.EBO);

            shader.use();

            glDrawElements(GL_TRIANGLES, _loader.meshes()[i].indexes().size(), GL_UNSIGNED_INT, 0);
        }
    }

private:
    std::vector<BufferUnion> bufferUnions;
};

#endif //CRD_MATERIALOPENGL_HPP
