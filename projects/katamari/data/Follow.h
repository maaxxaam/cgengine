#pragma once

#include "src/objects/components/base.h"
#include "src/objects/components/transform.h"
#include "src/update/update.h"

class Follow: public ComponentBase, public Update {
public:
    Follow(Object &self, watch_ptr<TransformComponent> target);

    MaybeError update(float delta) override;
private:
    watch_ptr<TransformComponent> _follower;
    watch_ptr<TransformComponent> _target;
    glm::vec3 _offsetPos, _offsetPYR;
};