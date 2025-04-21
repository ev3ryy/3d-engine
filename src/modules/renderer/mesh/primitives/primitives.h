#ifndef RENDERER_MESH_PRIMITIVES
#define RENDERER_MESH_PRIMITIVES

#include <vector>
#include <utility>
#include "mesh/mesh.h"

namespace primitives {

    inline std::pair<std::vector<vertex>, std::vector<uint32_t>> createCube() {
        std::vector<vertex> vertices = {
            {{-0.5f, -0.5f, -0.5f}}, // 0
            {{ 0.5f, -0.5f, -0.5f}}, // 1
            {{ 0.5f,  0.5f, -0.5f}}, // 2
            {{-0.5f,  0.5f, -0.5f}}, // 3
            {{-0.5f, -0.5f,  0.5f}}, // 4
            {{ 0.5f, -0.5f,  0.5f}}, // 5
            {{ 0.5f,  0.5f,  0.5f}}, // 6
            {{-0.5f,  0.5f,  0.5f}}  // 7
        };

        std::vector<uint32_t> indices = {
            0, 3, 2, 2, 1, 0,
            4, 5, 6, 6, 7, 4,
            0, 1, 5, 5, 4, 0,
            2, 3, 7, 7, 6, 2,
            0, 4, 7, 7, 3, 0,
            1, 2, 6, 6, 5, 1 
        };

        return { vertices, indices };
    }

    inline std::pair<std::vector<vertex>, std::vector<uint32_t>> createPyramid() {
        std::vector<vertex> vertices = {
            {{-0.5f, 0.f, -0.5f}}, // 0
            {{ 0.5f, 0.f, -0.5f}}, // 1
            {{ 0.5f, 0.f,  0.5f}}, // 2
            {{-0.5f, 0.f,  0.5f}}, // 3
            {{0.f, 1.f, 0.f}},     // 4
        };

        std::vector<uint32_t> indices = {
            0, 3, 2, 2, 1, 0,

            0, 4, 1,
            1, 4, 2,
            2, 4, 3,
            3, 4, 0,
        };

        return { vertices, indices };
    }
}

#endif // RENDERER_MESH_PRIMITIVES