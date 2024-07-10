#include "Spawner.h"

#include "src/random.h"

void Spawner::spawn() {
    float x = randFloat(-150.f, 150.f);
    float z = randFloat(-150.f, 150.f);
    float y = 7.5f;

    glm::mat4 transformMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3(x, y, z));

    std::string name = items[_index];
    _index = (_index + 1) % 3;
    fmt::println("New obj {}: {} {} {}", name, x, y, z);

    Object plane = _scene->addRenderObject(name).value();
    watch_ptr<TransformComponent> planeTransform = plane.addComponent<TransformComponent>(transformMatrix);
    JPH::ShapeRefC planeShape = JPH::BoxShapeSettings(JPH::Vec3(1.f, 1.f, 1.f)).Create().Get();
    watch_ptr<RigidBodyComponent> backRigid = plane.addComponent<RigidBodyComponent>(planeShape, planeTransform, RigidBodyType::Dynamic);
    backRigid->createAndAdd();

}