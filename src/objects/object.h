#pragma once

#include <entt/entt.hpp>

#include "level.h"
#include "src/watchptr.h"

class Object {
public:
    Object(Level* level, const entt::entity& id): _level(level), _id(id) {};
    Object(Level* level);
    Object(const Object &other): _level(other._level), _id(other._id) {};

    // Gets existing component for an object
    // WARNING: assumes component exists
    template<typename Component>
    watch_ptr<Component> getComponent() {
        return watch_ptr<Component>(_level->_registry.try_get<Component>(_id));
    }
    template<typename Component, typename... Args>
    watch_ptr<Component> addComponent(Args&&... args) {
        _level->_registry.emplace_or_replace<Component>(_id, *this, std::forward<Args>(args)...);
        return getComponent<Component>();
    }
    // Get object's component, else add a new one with parameters
    template<typename Component, typename... Args>
    watch_ptr<Component> getComponentDefault(Args&&... args) {
        if (hasComponent<Component>()) return getComponent<Component>();
        else return addComponent<Component>(std::forward<Args>(args)...);
    }
    // Removes an existing component
    // WARNING: assumes component exists, dunno otherwise
    template<typename Component>
    bool removeComponent() {
        return _level->_registry.remove<Component>(_id);
    }
    template<typename Component>
    bool hasComponent() const {
        return _level->_registry.all_of<Component>(_id);
    }

    Object another(entt::entity id) { return Object(_level, id); };

    entt::entity getID() const { return _id; };
private:
    entt::entity _id;
    Level* _level;
};