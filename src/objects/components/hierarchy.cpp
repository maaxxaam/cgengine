#include "hierarchy.h"
#include "src/error.h"
#include "src/objects/object.h"

MaybeError HierarchyComponent::setParent(Object parentTarget) {
    // TODO: check for cycles in hierarchy and error+abort when found
    _parentId = parentTarget.getID();
    watch_ptr<HierarchyComponent> parentHierarchy = parentTarget.getComponentDefault<HierarchyComponent>();
    return parentHierarchy->addChild(_self);
}

bool HierarchyComponent::hasChild(Object childTarget) {
    return std::find_if(children.begin(), children.end(), [&](entt::entity iter){ return (childTarget.getID()) == iter; }) != children.end();
}

MaybeError HierarchyComponent::addChild(Object childTarget) {
    // TODO: check for cycles in hierarchy and err+abort when found
    // TODO: make object printable with fmt
    if (hasChild(childTarget)) return std::nullopt;
    children.push_back(childTarget.getID());
    watch_ptr<HierarchyComponent> childH = childTarget.getComponentDefault<HierarchyComponent>();
    return childH->setParent(_self);
}

entityList HierarchyComponent::allChildren() {
    entityList result = children;
    entityList queue = children;
    while (!queue.empty()) {
        Object item = _self.another(queue.back());
        queue.pop_back();
        // Just skip objects without any hierarchy information (although how did that happen?)
        if (!item.hasComponent<HierarchyComponent>()) continue;
        watch_ptr<HierarchyComponent> itemHierarchy = item.getComponent<HierarchyComponent>();
        for (auto& child: itemHierarchy->children) {
            result.push_back(child);
            queue.push_back(child);
        }
    }
    return result;
}

entityList HierarchyComponent::leafChildren() {
    entityList result = children;
    entityList queue = children;
    while (!queue.empty()) {
        Object item = _self.another(queue.back());
        queue.pop_back();
        // Just skip objects without any hierarchy information 
        if (!item.hasComponent<HierarchyComponent>()) continue;
        watch_ptr<HierarchyComponent> itemHierarchy = item.getComponent<HierarchyComponent>();
        if (itemHierarchy->children.empty())
            result.push_back(item.getID());
        else
            for (auto& child: itemHierarchy->children) {
                queue.push_back(child);
            };
    }
    return result;
}
