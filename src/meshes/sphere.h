#pragma once

#include <glm/geometric.hpp>

#include <cmath>
#include <cstdint>
#include <vector>
#include <map>

#include "src/vk_mesh.h"

class SphereCreator {
public:
    Mesh create(int recursionLevel);

private:
    Mesh geometry;
    std::map<uint64_t, uint32_t> middlePointIndexCache;
    std::vector<uint32_t> indices;
    int index;
    glm::vec3 vertexColor;

    // add vertex to mesh, fix position to be on unit sphere, return index
    int addVertex(Vertex p) {
        Vertex pos;
        pos.position = glm::normalize(p.position);
        pos.normal = glm::normalize(pos.position);
        geometry._vertices.push_back(pos);
        return index++;
    }

    // return index of point in the middle of p1 and p2
    int getMiddlePoint(int p1, int p2) {
        // first check if we have it already
        bool firstIsSmaller = p1 < p2;
        uint64_t smallerIndex = firstIsSmaller ? p1 : p2;
        uint64_t greaterIndex = firstIsSmaller ? p2 : p1;
        uint64_t key = (smallerIndex << 32) + greaterIndex;

        if (middlePointIndexCache.find(key) != middlePointIndexCache.end()) {
            return middlePointIndexCache[key];
        }

        // not in cache, calculate it
        Vertex point1 = geometry._vertices[p1];
        Vertex point2 = geometry._vertices[p2];
        Vertex middle = {(point1.position + point2.position) / 2.0f, point1.color};

        // add vertex makes sure point is on unit sphere
        int i = addVertex(middle); 

        // store it, return index
        middlePointIndexCache.insert({key, i});
        return i;
    }
};
