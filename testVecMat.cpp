//
// Created by lc on 2022/3/29.
//

#include <core/Vec.hpp>
#include <core/Mat.hpp>
#include <object/Mesh.hpp>
#include <object/Triangle.hpp>
#include <renderer/OpenGLRenderer.hpp>
#include <scene/Loader.hpp>
#include <scene/Scene.hpp>
#include <shader/GLSLShader.hpp>

int main() {
    Loader spot = Loader("../models/spot/spot_triangulated_good.obj");
//    Loader box = Loader("../models/cornellbox/shortbox.obj");

    Scene scene;
//    scene.add(box.meshes());
    scene.add(spot.meshes());

    Camera camera(
        Vec3f { 0, 0, 2 },
        Vec3f { 0, 0, 0 },
        Vec3f { 0, 1, 0 }
    );

    OpenGLRenderer openGlRenderer = OpenGLRenderer::createOpenGLRenderer();
    openGlRenderer.render(scene, camera);

    return 0;
}