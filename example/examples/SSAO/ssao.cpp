//
// Created by lc on 2022/4/6.
//

#include <core/Vec.hpp>
#include <core/Mat.hpp>
#include <renderer/opengl/OpenGLRenderer.hpp>
#include <scene/Scene.hpp>
#include <shader/ShaderGLSL.hpp>
#include <scene/Camera.hpp>

#include <loader/Texture2DLoader.hpp>
#include <loader/MeshLoader.hpp>

int main() {
    const MeshLoader& spot = MeshLoader::load("../example/models/spot/spot_triangulated_good.obj");
    const MeshLoader& box = MeshLoader::load("../example/models/cornellbox/shortbox.obj");

    const Texture2DLoader& spotTexture = Texture2DLoader::load("../example/models/spot/spot_texture.png");

    Scene scene;

    scene.add(
            "spot",
            Material(
                    spot,
                    std::vector<const TextureLoader*>{ &spotTexture },
                    Mat4f::Identity()
            )
    );

//    scene.add("light", box, Mat4f::Translation(Vec3f { 0, 50, 300 }) * Mat4f::Scale(Vec3f { 0.1, 0.1, 0.1 }));
//    scene.add("box", box, Mat4f::Scale(Vec3f { 0.5, 0.5, 0.5 }));
//    scene.add("spot1", spot, Mat4f::Identity());
////    scene.add("spot2", spot, Mat4f::Translation(Vec3f { 0.5, 0, 2 }));
////    scene.add("spot3", spot, Mat4f::Translation(Vec3f { -0.5, 0, 4 }));
//    scene.add(
//            "floor",
//            box,
//            Mat4f::Translation(Vec3f { -100, 0, -200 })
//            * Mat4f::Scale(Vec3f { 1, 0.01, 1 })
//            * Mat4f::Flip(Vec3f { 0, 1, 0 })
//    );

    Camera camera(
            Vec3f { 0, 0, -10 },
            Vec3f { 0, 0, 0 },
            Vec3f { 0, 1, 0 }
    );

    ShaderGLSL shader(
            "../example/paperReproduction/Interactive Order-Independent Transparency/shaders/vertex.glsl",
            "../example/paperReproduction/Interactive Order-Independent Transparency/shaders/fragment.glsl"
    );
    OpenGLRenderer openGlRenderer = OpenGLRenderer::createOpenGLRenderer();

    openGlRenderer.render(
            scene,
            camera,
            [&shader](std::unordered_map<std::string, MaterialOpenGL>& materials) {
                shader.init();

                for (auto& material : materials) {
                    material.second.bind();
                }

                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
            },
            [&shader](
                    const Mat4f& view,
                    const Mat4f& projection,
                    std::unordered_map<std::string, MaterialOpenGL>& materials
            ) {
                glClearColor(0.f, 0.f, 0.f, 1.f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                shader.setUniform("uView", view);
                shader.setUniform("uProjection", projection);

                for (const auto& material : materials) {
                    if (material.first == "floor") {
                        shader.setUniform("uColor", Vec4f { 1.0, 1.0, 1.0, 1.0 });
                    } else {
                        shader.setUniform("uColor", Vec4f { 1.0f, 0.5f, 0.2f, 0.3 });
                    }
                    shader.setUniform("uModel", material.second.getTransformation());
                    material.second.draw(shader);
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