#pragma once
#include <vector>
#include <array>
#include "misc.h"
#include "structs.h"
#include <glm/glm.hpp>

/*const  std::vector < std::array<float, 3>> poits2d = {
        {0.25f, 0.25f, 0.0f},
        {0.75f, 0.26f, 0.0f},
        {0.75f, 0.24f, 0.0f},
        {0.25f, 0.72f, 0.0f},
        {0.27f, 0.75f, 0.0f},
        {0.23f, 0.77f, 0.0f},

}*/;

const std::vector<std::array<float, 3>> poits2d =  Misc::seedUniformPoints2D(80);
const uint32_t NUM_ELEMENTS = size(poits2d);
const uint32_t NUM_LBVH_ELEMENTS = NUM_ELEMENTS + NUM_ELEMENTS - 1;
const uint32_t NUM_BB_POINTS = (NUM_ELEMENTS - 1) * 12 * 2;
                            // num internal nodes * num box edges * num points
const VkDeviceSize lineVertSize = sizeof(VertexBase) * (NUM_ELEMENTS - 1) * 12 * 2;
class BBox {
private:
  /*  float minx;
    float miny;
    float minz;
    float maxx;
    float maxy;
    float maxz;
    glm::vec3 p000;
    glm::vec3 p100;
    glm::vec3 p110; 
    glm::vec3 p010;
    glm::vec3 p001;
    glm::vec3 p101;
    glm::vec3 p111;
    glm::vec3 p011;*/
    //std::vector<std::array< glm::vec3, 2>> lines;
    //glm::vec4 clr;

public:
    //std::vector<VertexBase> vertices;
    static void getVerts(
        float minx, float miny, float minz,
        float maxx, float maxy, float maxz,
        std::vector<VertexBase>& pVerts);

};