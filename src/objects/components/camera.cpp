#include "camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

Camera::Camera(const Object &self, glm::vec3 position, glm::vec3 orientationPYR, glm::vec2 viewportSize, CameraPurpose purpose):
    ComponentBase(self) {
    _position = position;
    _orientationPYR = orientationPYR;
    _projectionType = CameraProjection::Perspective;
    _purpose = purpose;
    _viewportSize = viewportSize;
    updateProjection();
    updateRotation();
    updateCameraData();
}

void Camera::setProjection(CameraProjection type) {
    _projectionType = type;
    updateProjection();
    updateCameraData();
}

void Camera::updateProjection() {
    switch (_projectionType) {
        case CameraProjection::Orthographic:
            _data.proj = glm::ortho(-_viewportSize.x/2, _viewportSize.x/2, -_viewportSize.y/2, -_viewportSize.y/2, 1e-2f, 1e4f);
            break;
        case CameraProjection::Perspective:
            _data.proj = glm::perspective(glm::radians(70.0f), _viewportSize.x / _viewportSize.y, 1e-2f, 1e5f);
            // Flip projection according to OpenGL matrices convention?
	        _data.proj[1][1] *= -1;	
            break;
    }
}

void Camera::updateRotation() {
    _rotation.x = glm::cos(_orientationPYR.y) * glm::cos(_orientationPYR.x);
    _rotation.y = glm::sin(_orientationPYR.y);
    _rotation.z = glm::sin(_orientationPYR.x) * glm::cos(_orientationPYR.y);
}

void Camera::updateCameraData() {
    _data.view = glm::lookAt(_position, _position + _rotation, glm::vec3(0, 1, 0));
    _data.viewproj = _data.proj * _data.view;
}

void Camera::setPosition(glm::vec3 position) {
    _position = position;
    updateCameraData();
}

void Camera::setOrientation(glm::vec3 orientationPYR) {
    _orientationPYR = orientationPYR;
    updateRotation();
    updateCameraData();
}
