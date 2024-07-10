#pragma once

#include "src/vector.h"
#include "src/scene.h"

class Spawner {
public:
    Spawner(Scene* scene): _scene(scene) {
        items[0] = LIGHT;
        // items[1] = WOLF;
        items[1] = TREE;
        items[2] = MAIL;
    };

    const char* LIGHT = "lighthouse";
    // const char* WOLF = "wolf";
    const char* TREE = "tree";
    const char* MAIL = "mailbox";

    void spawn();
private:
    void createObject(const char* name, glm::vec3 position);

    int _index = 0;
    Scene* _scene;
    const char* items[3];
};
