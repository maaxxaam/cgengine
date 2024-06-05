#pragma once

#include "base.h"

class RenderObject: public ComponentBase {
public:
    RenderObject(const Object &self, Mesh* _mesh, Material* _material):
        ComponentBase(self), mesh(_mesh), material(_material) {};
	Mesh* mesh;
	Material* material;
};
