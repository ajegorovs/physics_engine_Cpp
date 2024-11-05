// https ://developer.nvidia.com/blog/parallelforall/wp-content/uploads/2012/11/karras2012hpg_paper.pdf
// https://github.com/MircoWerner/VkLBVH

#pragma once
#include <vector>
#include <array>
#include "misc.h"
#include "structs.h"
#include <glm/glm.hpp>

const float P_R = 0.1f;
//const std::vector<glm::vec3> poits2d =  Misc::seedUniformPoints2D(50);
//const uint32_t NUM_ELEMENTS = static_cast<uint32_t>(size(poits2d));
const uint32_t NUM_ELEMENTS = 1000;// glm::pow(10, 3);
const uint32_t NUM_LBVH_ELEMENTS = NUM_ELEMENTS + NUM_ELEMENTS - 1;
const uint32_t NUM_BB_POINTS = ((NUM_ELEMENTS -1) + 1) * 12 * 2; // + external box
                            // num internal nodes * num box edges * num points
const VkDeviceSize lineVertSize = sizeof(VertexBase) * NUM_BB_POINTS;

//const std::vector<float> asd = Misc::getExtent(poits2d);
//const PushConstantsMortonCodes pushConstMC{ NUM_ELEMENTS, asd[0], asd[1], asd[2], asd[3], asd[4], asd[5]};
const PushConstantsMortonCodes pushConstMC{ NUM_ELEMENTS, -P_R , -P_R, -P_R , 1.0f + P_R , 1.0f + P_R , 1.0f + P_R };
const PushConstantsRadixSort pushConstRS{ NUM_ELEMENTS };
const PushConstantsHierarchy pushConstHierarchy{ NUM_ELEMENTS, 1 };
const PushConstantsBoundingBoxes pushConstantsBoundingBoxes{ NUM_ELEMENTS, 1 };


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


public:
    static void getVerts(
        glm::vec3 color,
        float minx, float miny, float minz,
        float maxx, float maxy, float maxz,
        std::vector<VertexBase>& pVerts);

};