#pragma once

#include "base.h"
#include "camera.h"
#include "transform.h"
#include "src/update/update.h"
#include "src/events/eventlistener.h"

class OrbitalCamera: public ComponentBase, public Update, public Events::EventListener {
public:
    float moveFactor, rotateFactor;

    OrbitalCamera(const Object &self, watch_ptr<Camera> camera, watch_ptr<TransformComponent> origin);
    OrbitalCamera& operator=(OrbitalCamera &&other);

    virtual void initListener() override;

    MaybeError update(float delta) override;
private:
	void handleRotation(const Events::Event &event);
	glm::vec3 calcOrbitPosition(float radius = 10.f);

    struct Lerpable {
        glm::vec3 current, target;
    };

    watch_ptr<Camera> _cam;
    watch_ptr<TransformComponent> _orbitOrigin;
    glm::vec3 _PYR, _position;
    glm::vec2 _rotationInput;
    Lerpable _origin;
    const float _orbitRadiusBase = 10.f;

    // Event references
    std::string _rotateMouseStr, _rotateLeftStr, _rotateRightStr, _rotateUpStr, _rotateDownStr;
    uint32_t _rotateMouseHash, _rotateLeftHash, _rotateRightHash, _rotateUpHash, _rotateDownHash;

    bool _clearRotation;
    const glm::vec3 _up {0.f, 1.f, 0.f};
};
