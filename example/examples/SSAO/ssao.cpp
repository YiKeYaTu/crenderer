//
// Created by lc on 2022/4/6.
//

#include <core/Vec.hpp>
#include <core/Mat.hpp>
#include <renderer/OpenGLRenderer.hpp>
#include <scene/Scene.hpp>
#include <shader/GLSLShader.hpp>
#include <scene/Camera.hpp>
#include <renderer/vendor/OpenGL.hpp>

#include <loader/Texture2DLoader.hpp>
#include <loader/MeshLoader.hpp>

#ifdef __APPLE__
#define SCALE 2.0
#else
#define SCALE 1.0
#endif

#define TEXTURE_WIDTH 1024
#define TEXTURE_HEIGHT 1024

std::vector<Vec3f> vertexes = {
        Vec3f { -1, 1, 0 },
        Vec3f { -1, -1, 0 },
        Vec3f { 1, -1, 0 },
        Vec3f { -1, 1, 0 },
        Vec3f { 1, -1, 0 },
        Vec3f { 1, 1, 0 },
};
std::vector<unsigned int> indexes = { 0, 1, 2, 3, 4, 5 };
std::vector<Vec3f> normals(6);
std::vector<Vec2f> texCoords = {
        Vec2f { 0, 1 },
        Vec2f { 0, 0 },
        Vec2f { 1, 0 },
        Vec2f { 0, 1 },
        Vec2f { 1, 0 },
        Vec2f { 1, 1 },
};

Mesh<Triangle> quadMesh(vertexes, indexes, normals, texCoords);

int main() {
    const MeshLoader& spot = MeshLoader::load("../example/models/spot/spot_triangulated_good.obj");
    const MeshLoader& box = MeshLoader::load("../example/models/cornellbox/shortbox.obj");
    const MeshLoader quad = MeshLoader(std::vector<Mesh<Triangle>>{ quadMesh });
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
    scene.add("quad", Material(quad));

    scene.add(
        "light",
        Material(
            box,
            std::vector<const TextureLoader*>{ },
            Mat4f::Translation(Vec3f { 0, 50, 300 }) * Mat4f::Scale(Vec3f { 0.1, 0.1, 0.1 })
        )
    );
//    scene.add("box", box, Mat4f::Scale(Vec3f { 0.5, 0.5, 0.5 }));
//    scene.add("spot1", spot, Mat4f::Identity());
//    scene.add("spot2", spot, Mat4f::Translation(Vec3f { 0.5, 0, 2 }));
//    scene.add("spot3", spot, Mat4f::Translation(Vec3f { -0.5, 0, 4 }));
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

    GLSLShader spotShader(
            "../example/examples/SSAO/shaders/vertex.glsl",
            "../example/examples/SSAO/shaders/fragment.glsl",
            "../example/examples/SSAO/shaders/geometry.glsl"
    );

    GLSLShader lightShader(
            "../example/examples/SSAO/shaders/vertex.glsl",
            "../example/examples/SSAO/shaders/lightFragment.glsl"
    );

    GLSLShader quadShader(
            "../example/examples/SSAO/shaders/quadVertex.glsl",
            "../example/examples/SSAO/shaders/quadFragment.glsl"
    );

    OpenGL::OpenGLFrameBuffer framebuffer;
    OpenGL::OpenGLTextureBuffer texColorBuffer;
    OpenGL::OpenGLRenderBuffer rbo;

    OpenGLRenderer openGlRenderer = OpenGLRenderer::createOpenGLRenderer("", scene);
    openGlRenderer.render(
            camera,
            [&](std::unordered_map<std::string, MaterialOpenGL>& materials) {
                spotShader.init();
                quadShader.init();
                lightShader.init();

                for (auto& material : materials) {
                    material.second.bind();
                }

                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);

                glEnable(GL_MULTISAMPLE);

                texColorBuffer = OpenGL::createRGBTexture2D(TEXTURE_WIDTH,  TEXTURE_HEIGHT);
                rbo = OpenGL::createRenderBuffer(TEXTURE_WIDTH, TEXTURE_HEIGHT);
                framebuffer = OpenGL::createFrameBuffer(texColorBuffer, rbo);
            },
            [&](
                int width, int height,
                const Mat4f& view,
                const Mat4f& projection,
                std::unordered_map<std::string, MaterialOpenGL>& materials
            ) {
                OpenGL::clearColor();
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                OpenGL::Window::setViewPort(TEXTURE_HEIGHT, TEXTURE_WIDTH);
                glEnable(GL_DEPTH_TEST);
                OpenGL::use(framebuffer);
                OpenGL::clearColor();
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                auto& spot = materials.find("spot")->second;
                spotShader.setUniform("uView", view);
                spotShader.setUniform("uProjection", camera.calcProjectionMatrix(static_cast<float>(width) / height));
                spotShader.setUniform("uModel", spot.getTransformation());
                spotShader.setUniform("time", glfwGetTime());
                spot.draw(spotShader, [&](unsigned int TEX) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, TEX);
                    spotShader.setUniform("uTexture", 0);
                });

                OpenGL::Window::setViewPort(width, height);
                OpenGL::useDefault(framebuffer);
                OpenGL::clearColor();
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                auto& quad = materials.find("quad")->second;
                quad.draw(quadShader, [&](unsigned int TEX) {
                    glActiveTexture(GL_TEXTURE0);
                    OpenGL::use(texColorBuffer);
                    quadShader.setUniform("uTexture", 0);
                });
            },
            [](std::unordered_map<std::string, MaterialOpenGL>& materials) {
                for (auto& material : materials) {
                    material.second.unbind();
                }
            }
    );

    return 0;
}