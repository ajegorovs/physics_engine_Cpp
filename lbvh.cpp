#include "lbvh.h"

void BBox::getVerts(
    float minx, float miny, float minz, 
    float maxx, float maxy, float maxz, 
    std::vector<VertexBase>& pVerts)
{
    glm::vec3 p000 = glm::vec3(minx, miny, minz);
    glm::vec3 p100 = glm::vec3(maxx, miny, minz);
    glm::vec3 p110 = glm::vec3(maxx, maxy, minz);
    glm::vec3 p010 = glm::vec3(minx, maxy, minz);
    glm::vec3 p001 = glm::vec3(minx, miny, maxz);
    glm::vec3 p101 = glm::vec3(maxx, miny, maxz);
    glm::vec3 p111 = glm::vec3(maxx, maxy, maxz);
    glm::vec3 p011 = glm::vec3(minx, maxy, maxz);

    std::vector<glm::vec3> pts = {
        p000, p100,
        p100, p110,
        p110, p010,
        p010, p000,
        p000, p001,
        p001, p101,
        p101, p111,
        p111, p011,
        p011, p001,
        p010, p011,
        p100, p101,
        p110, p111
    };

    for (glm::vec3 p : pts)
    {   
        VertexBase v{ p, glm::vec3(0.0f, 1.0f, 0.0f) };
        pVerts.push_back(v);
    }

}
