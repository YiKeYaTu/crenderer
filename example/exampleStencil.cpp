//
// Created by lc on 2022/3/29.
//

#include <core/Vec.hpp>
#include <core/Mat.hpp>
#include <renderer/opengl/OpenGLRenderer.hpp>
#include <scene/Loader.hpp>
#include <scene/Scene.hpp>
#include <shader/ShaderGLSL.hpp>
#include <scene/Camera.hpp>

int main() {
    const Loader& spot = Loader::load("../example/models/spot/spot_triangulated_good.obj");
    const Loader& box = Loader::load("../example/models/cornellbox/shortbox.obj");

    Scene scene;

    scene.add("light", box, Mat4f::Translation(Vec3f { 0, 50, 300 }) * Mat4f::Scale(Vec3f { 0.1, 0.1, 0.1 }));
    scene.add("box", box, Mat4f::Scale(Vec3f { 0.5, 0.5, 0.5 }));
    scene.add("spot", spot, Mat4f::Identity());
    scene.add(
        "floor",
        box,
        Mat4f::Translation(Vec3f { -100, 0, -200 })
            * Mat4f::Scale(Vec3f { 1, 0.01, 1 })
            * Mat4f::Flip(Vec3f { 0, 1, 0 })
    );

    Camera camera(
        Vec3f { 0, 0, -10 },
        Vec3f { 0, 0, 0 },
        Vec3f { 0, 1, 0 }
    );

    ShaderGLSL shader("../example/shaders/vertexShader.glsl", "../example/shaders/fragmentShader.glsl");
    ShaderGLSL singleColorShader("../example/shaders/singleColorVertexShader.glsl", "../example/shaders/singleColorFragmentShader"
                                                                         ".glsl");
    OpenGLRenderer openGlRenderer = OpenGLRenderer::createOpenGLRenderer();

    openGlRenderer.render(
        scene,
        camera,
        [&shader, &singleColorShader](std::unordered_map<std::string, MaterialOpenGL>& materials) {
            shader.init();
            singleColorShader.init();

            for (auto& material : materials) {
                material.second.bind();
            }

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);

            glEnable(GL_STENCIL_TEST);

            glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
        },
        [&shader, &singleColorShader](
            const Mat4f& view,
            const Mat4f& projection,
            std::unordered_map<std::string, MaterialOpenGL>& materials
        ) {
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            shader.setUniform("uView", view);
            shader.setUniform("uProjection", projection);

            for (const auto& material : materials) {
                glStencilMask(0xFF);
                glClear(GL_STENCIL_BUFFER_BIT);
                glStencilFunc(GL_ALWAYS, 1, 0xFF);

                if (material.first == "floor") {
                    shader.setUniform("uColor", Vec4f { 1.0, 1.0, 1.0, 1.0 });
                } else {
                    shader.setUniform("uColor", Vec4f { 1.0f, 0.5f, 0.2f, 1.0 });
                }
                shader.setUniform("uModel", material.second.getTransformation());

                material.second.draw(shader);

                if (material.first == "spot") {
                    singleColorShader.setUniform("uView", view);
                    singleColorShader.setUniform("uProjection", projection);
                    singleColorShader.setUniform("uModel", Mat4f::Identity());
                    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
                    glStencilMask(0x00);
                    material.second.draw(singleColorShader);
                }
            }
        },
        [](std::unordered_map<std::string, MaterialOpenGL>& materials) {
            for (auto& material : materials) {
                material.second.unbind();
            }
        }
    );

    return 0;
}