#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <Jolt/Jolt.h>

class Vec3 {
public:
    /*template<typename... Args>
    Vec3(Args&&... args): _vec(std::forward(args)...) {};*/
    Vec3(glm::vec3 vec): _vec(vec) {};
    Vec3(): _vec(glm::zero<glm::vec3>()) {};

    Vec3(const JPH::Vec3& vec): _vec(vec.GetX(), vec.GetY(), vec.GetZ()) {};
    glm::vec3* operator->() { return &_vec; };
    JPH::Vec3 jolt() const { return JPH::Vec3(_vec.x, _vec.y, _vec.z); };
    glm::vec3 glm() const { return _vec; };
    operator glm::vec3() const { return glm(); };
    operator JPH::Vec3() const { return jolt(); };
private:
    glm::vec3 _vec;
};
