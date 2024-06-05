#include "object.h"
#include "level.h"

Level::Level() {
}

Object Level::addObject() {
    return Object(this, _registry.create());
}

Object Level::getObject(entt::entity id) {
    return Object(this, id);
}
