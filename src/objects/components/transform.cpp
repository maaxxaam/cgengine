#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include "transform.h"
#include "src/gpustructs.h"

const glm::vec3 TransformComponent::getTranslation() { 
    if (_dirtyData) {
        decompose();
        _dirtyData = false;
    }
    return _translation; 
};

const glm::vec3 TransformComponent::getScale() { 
    if (_dirtyData) {
        decompose();
        _dirtyData = false;
    }
    return _scale; 
};

const glm::vec3 TransformComponent::getSkew() { 
    if (_dirtyData) {
        decompose();
        _dirtyData = false;
    }
    return _skew; 
};

const glm::quat TransformComponent::getOrientation() { 
    if (_dirtyData) {
        decompose();
        _dirtyData = false;
    }
    return _orientation; 
};

const glm::vec3 TransformComponent::getEulerOrientation() {
    if (_dirtyData) {
        decompose();
        _dirtyData = false;
    }
    return _euler;
};

const glm::vec4 TransformComponent::getPerspective() { 
    if (_dirtyData) {
        decompose();
        _dirtyData = false;
    }
    return _perspective; 
};

const JPH::RMat44 TransformComponent::getJoltTransform() {
    recalcIfDirty();
    return JPH::Mat44::sLoadFloat4x4(reinterpret_cast<JPH::Float4*>(glm::value_ptr(_fullMatrix)));
}

const glm::mat4 TransformComponent::getMatrix() {
    recalcIfDirty();
    return _fullMatrix;
}

const GPUObjectData TransformComponent::getGPUMatrix() {
    recalcIfDirty();
    return {_fullMatrix};
}

void TransformComponent::setTranslation(const glm::vec3 &translation) {
    if (_dirtyData) decompose();
    _dirtyMatrix = true;
    _dirtyData = false;
    _translation = translation;
};

void TransformComponent::setScale(const glm::vec3 &scale) {
    if (_dirtyData) decompose();
    _dirtyMatrix = true;
    _dirtyData = false;
    _scale = scale;
};

void TransformComponent::setSkew(const glm::vec3 &skew) {
    if (_dirtyData) decompose();
    _dirtyMatrix = true;
    _dirtyData = false;
    _skew = skew;
};

void TransformComponent::setOrientation(const glm::quat &orientation) {
    if (_dirtyData) decompose();
    _dirtyMatrix = true;
    _dirtyData = false;
    _orientation = orientation;
    _euler = glm::eulerAngles(_orientation);
};

void TransformComponent::setEulerOrientation(const glm::vec3 &euler) {
    if (_dirtyData) decompose();
    _dirtyMatrix = true;
    _dirtyData = false;
    _euler = euler;
    _orientation = glm::toQuat(glm::orientate3(_euler));
};

void TransformComponent::setPerspective(const glm::vec4 &perspective) {
    if (_dirtyData) decompose();
    _dirtyMatrix = true;
    _dirtyData = false;
    _perspective = perspective;
};

void TransformComponent::setMatrix(const glm::mat4 &matrix) {
    _dirtyData = true;
    _dirtyMatrix = false;
    _fullMatrix = matrix;
}

void TransformComponent::setMatrix(JPH::Mat44Arg &matrix) {
    _dirtyData = true;
    _dirtyMatrix = false;
    float * temp = new float[16];
    matrix.StoreFloat4x4(reinterpret_cast<JPH::Float4*>(temp));
    _fullMatrix = glm::make_mat4x4(temp);
}

void TransformComponent::setIdentity() {
    _dirtyData = true;
    _dirtyMatrix = false;
    _fullMatrix = glm::identity<glm::mat4>();
}

void TransformComponent::decompose() {
	glm::decompose(_fullMatrix, _scale, _orientation, _translation, _skew, _perspective);
    glm::extractEulerAngleXYZ(_fullMatrix, _euler.x, _euler.y, _euler.z);
}

void TransformComponent::recalcMatrix() {
    _fullMatrix = glm::mat4(1.0f);

    _fullMatrix[0][3] = _perspective.x;
    _fullMatrix[1][3] = _perspective.y;
    _fullMatrix[2][3] = _perspective.z;
    _fullMatrix[3][3] = _perspective.w;

    _fullMatrix *= glm::translate(_translation);
    _fullMatrix *= glm::mat4_cast(_orientation);

    if (_skew.x) {
        glm::mat4 tmp { 1.f };
        tmp[2][1] = _skew.x;
        _fullMatrix *= tmp;
    }

    if (_skew.y) {
        glm::mat4 tmp { 1.f };
        tmp[2][0] = _skew.y;
        _fullMatrix *= tmp;
    }

    if (_skew.z) {
        glm::mat4 tmp { 1.f };
        tmp[1][0] = _skew.z;
        _fullMatrix *= tmp;
    }

    _fullMatrix *= glm::scale(_scale);
}

void TransformComponent::recalcIfDirty() {
    if (_dirtyMatrix) {
        recalcMatrix();
        _dirtyMatrix = false;
    }
}