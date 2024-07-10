#pragma once

#include <bitset>
#include <vector>

#include "base.h"
#include "src/error.h"
#include "src/objects/object.h"

using entityList = std::vector<entt::entity>;

enum class InheritanceFlag {
    LOCAL_TRANSFORM = 0,
};

class HierarchyComponent: ComponentBase {
public:
    HierarchyComponent(const Object &self, const entt::entity parent = entt::null):
        ComponentBase(self), _parentId(parent) {};
    
    // Hierarchy helpers
    MaybeError setParent(Object parentTarget);
    entt::entity getParent() { return _parentId; };
    bool hasRootParent() { return _parentId == entt::null; };

	bool hasChild(Object childTarget);
    MaybeError addChild(Object childTarget);
    MaybeError removeChild(Object childTarget);

    // Get different children as vectors
    entityList getChildren() { return children; };
    entityList allChildren();
    entityList leafChildren();

    entt::entity _parentId = entt::null;
    // std::bitset<32> _inheritanceFlags;
    entityList children;    
};
