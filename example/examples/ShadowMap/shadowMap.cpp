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

    const Vec3f lightPosition { 0, 30, 30 };

    Scene scene;
    scene.add("quad", Material(quad));
    scene.add(
        "spot",
        Material(
            spot,
            std::vector<const TextureLoader*>{ &spotTexture },
            Mat4f::Identity()
        )
    );
    scene.add(
        "light",
        Material(
            box,
            std::vector<const TextureLoader*>{ },
            Mat4f::Translation(lightPosition) * Mat4f::Scale(Vec3f { 0.01, 0.01, 0.01 })
        )
    );

    scene.add(
        "floor",
        Material(
            box,
            std::vector<const TextureLoader*>{ },
            Mat4f::Translation(Vec3f { -20, -2.35, -20 })
            * Mat4f::Scale(Vec3f { 0.1, 0.01, 0.1 })
        )
    );

    Camera camera(
        Vec3f { 0, 0, -10 },
        Vec3f { 0, 0, 0 },
        Vec3f { 0, 1, 0 }
    );

    Camera lightCamera(
        lightPosition,
        Vec3f { 0, 0, 0 },
        Vec3f { 0, 1, 0 }
    );

    GLSLShader spotShader("../example/examples/ShadowMap/shaders/common.vs.glsl", "../example/examples/ShadowMap/shaders/spot.fs.glsl");
    GLSLShader shadowMapShader("../example/examples/ShadowMap/shaders/common.vs.glsl", "../example/examples/ShadowMap/shaders/shadowMap.fs.glsl");
    GLSLShader lightShader("../example/examples/ShadowMap/shaders/common.vs.glsl", "../example/examples/ShadowMap/shaders/light.fs.glsl");
    GLSLShader floorShader("../example/examples/ShadowMap/shaders/common.vs.glsl", "../example/examples/ShadowMap/shaders/floor.fs.glsl");
    GLSLShader quadShader("../example/examples/ShadowMap/shaders/quad.vs.glsl", "../example/examples/ShadowMap/shaders/quad.fs.glsl");

    OpenGL::OpenGLFrameBuffer framebuffer;
    OpenGL::OpenGLTextureBuffer texColorBuffer;
    OpenGL::OpenGLRenderBuffer rbo;

    OpenGLRenderer openGlRenderer = OpenGLRenderer::createOpenGLRenderer("", scene);
    openGlRenderer.render(
            camera,
//            lightCamera,
            [&](std::unordered_map<std::string, MaterialOpenGL>& materials) {
                spotShader.init();
                shadowMapShader.init();
                floorShader.init();
                lightShader.init();
                quadShader.init();

                for (auto& material : materials) {
                    material.second.bind();
                }

                OpenGL::enable(GL_DEPTH_TEST);
                OpenGL::enable(GL_MULTISAMPLE);

                texColorBuffer = OpenGL::createFloat32RedTexture2D(TEXTURE_WIDTH,  TEXTURE_HEIGHT);
                rbo = OpenGL::createRenderBuffer(TEXTURE_WIDTH, TEXTURE_HEIGHT);
                framebuffer = OpenGL::createFrameBuffer(texColorBuffer, rbo);
            },
            [&](
                    int width, int height,
                    const Mat4f& view,
                    const Mat4f& projection,
                    std::unordered_map<std::string, MaterialOpenGL>& materials
            ) {
                OpenGL::Window::setViewPort(TEXTURE_HEIGHT, TEXTURE_WIDTH);
                OpenGL::use(framebuffer);
                OpenGL::clearColor();
                OpenGL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                auto& spot = materials.find("spot")->second;
                shadowMapShader.setUniform("uView", lightCamera.calcViewMatrix());
                shadowMapShader.setUniform("uProjection", lightCamera.calcOrthogonalProjectionMatrix());
                shadowMapShader.setUniform("uModel", spot.getTransformation());
                spot.draw(shadowMapShader);

                auto& floor = materials.find("floor")->second;
                shadowMapShader.setUniform("uModel", floor.getTransformation());
                floor.draw(shadowMapShader);

                OpenGL::Window::setViewPort(width, height);
                OpenGL::useDefault(framebuffer);
                OpenGL::clearColor();
                OpenGL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                spotShader.setUniform("uView", view);
                spotShader.setUniform("uProjection", camera.calcProjectionMatrix(static_cast<float>(width) / height));

                spotShader.setUniform("uModel", spot.getTransformation());
                spotShader.setUniform("uLightPosition", lightPosition);
                spot.draw(spotShader, [&](unsigned int TEX) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, TEX);
                    spotShader.setUniform("uTexture", 0);
                });

                floorShader.setUniform("uLightView", lightCamera.calcViewMatrix());
                floorShader.setUniform("uLightProjection", lightCamera.calcOrthogonalProjectionMatrix());

                floorShader.setUniform("uView", view);
                floorShader.setUniform("uProjection", camera.calcProjectionMatrix(static_cast<float>(width) / height));
                floorShader.setUniform("uModel", floor.getTransformation());
                glActiveTexture(GL_TEXTURE0);
                OpenGL::use(texColorBuffer);
                floorShader.setUniform("uShadowMap", 0);

                floor.draw(floorShader);

                auto& light = materials.find("light")->second;
                lightShader.setUniform("uView", view);
                lightShader.setUniform("uProjection", camera.calcProjectionMatrix(static_cast<float>(width) / height));

                lightShader.setUniform("uModel", light.getTransformation());
                floor.draw(lightShader);
            },
            [](std::unordered_map<std::string, MaterialOpenGL>& materials) {
                for (auto& material : materials) {
                    material.second.unbind();
                }
            }
    );

    return 0;
}