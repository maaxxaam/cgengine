#include "sphere.h"

Mesh SphereCreator::create(int recursionLevel) {
    geometry = Mesh{};
    middlePointIndexCache.clear();
    index = 0;

    // create 12 vertices of a icosahedron
    int t = (1.0 + std::sqrt(5.0)) / 2.0;

    addVertex({glm::vec3(-1,  t,  0)});
    addVertex({glm::vec3( 1,  t,  0)});
    addVertex({glm::vec3(-1, -t,  0)});
    addVertex({glm::vec3( 1, -t,  0)});

    addVertex({glm::vec3( 0, -1,  t)});
    addVertex({glm::vec3( 0,  1,  t)});
    addVertex({glm::vec3( 0, -1, -t)});
    addVertex({glm::vec3( 0,  1, -t)});

    addVertex({glm::vec3( t,  0, -1)});
    addVertex({glm::vec3( t,  0,  1)});
    addVertex({glm::vec3(-t,  0, -1)});
    addVertex({glm::vec3(-t,  0,  1)});

    // create 20 triangles of the icosahedron

    indices.insert(indices.end(), {
        // 5 faces around point 0
        0, 11, 5,
        0, 5, 1,
        0, 1, 7,
        0, 7, 10,
        0, 10, 11,

        // 5 adjacent faces 
        1, 5, 9,
        5, 11, 4,
        11, 10, 2,
        10, 7, 6,
        7, 1, 8,

        // 5 faces around point 3
        3, 9, 4,
        3, 4, 2,
        3, 2, 6,
        3, 6, 8,
        3, 8, 9,

        // 5 adjacent faces 
        4, 9, 5,
        2, 4, 11,
        6, 2, 10,
        8, 6, 7,
        9, 8, 1 }
    );

    // refine triangles
    for (int i = 0; i < recursionLevel; i++)
    {
        std::vector<uint32_t> indices2;
        for (int i = 0; i < indices.size(); i += 3) {
            // replace triangle by 4 triangles
            uint32_t v1 = indices[i];
            uint32_t v2 = indices[i + 1];
            uint32_t v3 = indices[i + 2];
            uint32_t a = getMiddlePoint(v1, v2);
            uint32_t b = getMiddlePoint(v2, v3);
            uint32_t c = getMiddlePoint(v3, v1);

            indices2.insert(indices2.end(), {
                v1, a, c,
                v2, b, a,
                v3, c, b,
                a, b, c }
            );
        }
        indices = indices2;
    }

    // done, now add triangles to mesh
    // geometry._indices = indices;
    Mesh result;
    for (auto & indice: indices) {
        result._vertices.push_back(geometry._vertices[indice]);
    }

    return result;
}
