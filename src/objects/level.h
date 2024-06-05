#pragma once

#include <entt/entt.hpp>

#include "src/gpustructs.h"
#include "src/update/update.h"

class Object;

using ObjectId = uint32_t;

template<typename Component>
using SimpleView = entt::basic_view<entt::get_t<entt::storage_for_t<Component>>, entt::exclude_t<>>;

class Level {
public:
    Level();

    Object getObject(entt::entity id);

    Object addObject();

    entt::registry _registry;
};
