#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Jolt/Jolt.h>

#include <utility>

#include "base.h"
#include "src/gpustructs.h"

struct TransformComponent: public ComponentBase {
    // template<typename... Args>
    // TransformComponent(Args&&... args): _fullMatrix(std::forward(args)...) {};
    TransformComponent(const Object &self, glm::mat4 matrix): ComponentBase(self), _fullMatrix(matrix) { decompose(); };
    TransformComponent(const Object &self): ComponentBase(self), _fullMatrix(1.0f) { decompose(); };
    TransformComponent(TransformComponent& other): ComponentBase(other._self), _fullMatrix(other._fullMatrix) { decompose(); };

    const glm::vec3 getTranslation();
    const glm::vec3 getScale();
    const glm::vec3 getSkew();
    const glm::quat getOrientation();
    const glm::vec3 getEulerOrientation();
    const glm::vec4 getPerspective();
    const glm::mat4 getMatrix();
    const JPH::RMat44 getJoltTransform();
    const GPUObjectData getGPUMatrix();

    void setTranslation(const glm::vec3 &translation);
    void setScale(const glm::vec3 &scale);
    void setSkew(const glm::vec3 &skew);
    void setOrientation(const glm::quat &orientation);
    void setEulerOrientation(const glm::vec3 &euler);
    void setPerspective(const glm::vec4 &perspective);
    void setMatrix(const glm::mat4 &matrix);
    void setMatrix(JPH::Mat44Arg &matrix);
    void setIdentity();

    operator glm::mat4() { return getMatrix(); };
    operator JPH::RMat44() { return getJoltTransform(); };
    operator GPUObjectData() { return getGPUMatrix(); };
    TransformComponent& operator=(glm::mat4 &matrix) { setMatrix(matrix); return *this; };
    TransformComponent& operator=(JPH::Mat44Arg &matrix) { setMatrix(matrix); return *this; };
private:
    glm::mat4 _fullMatrix;

    glm::vec3 _translation, _scale, _skew, _euler;
    glm::vec4 _perspective;
    glm::quat _orientation;

    bool _dirtyMatrix, _dirtyData;

    void recalcMatrix();
    inline void recalcIfDirty();
    void decompose();
};
