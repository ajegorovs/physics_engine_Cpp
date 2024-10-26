#include "shapes.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp> 
#include "glm/ext.hpp"
#include <cmath> // idk 
#include <iostream>

Plane::Plane(glm::vec3 s, glm::vec3 c, glm::vec2 rotZY) : geometric_shape("Plane", c, rotZY), sides(s) {
    vertices.insert(vertices.end(), {
                                    {-0.5f,-0.5f, 0.0f, 1.f},
                                    {0.5f, -0.5f, 0.0f, 1.f},
                                    {0.5f,  0.5f, 0.0f, 1.f},
                                    {-0.5f, 0.5f, 0.0f, 1.f}
        });

    transform(vertices.size(), rotZY, sides);
    indices.insert(indices.end(), { 0,1,2,2,3,0 });
};

Prism::Prism(glm::vec3 s, glm::vec3 c, glm::vec2 rotZY) : geometric_shape("Prism", c, rotZY), sides(s) {

    vertices.insert(vertices.end(), { {-0.5f, -0.5f, -0.5f, 1.f},{ 0.5f, -0.5f, -0.5f, 1.f},
                                    { 0.5f,  0.5f, -0.5f, 1.f}, {-0.5f,  0.5f, -0.5f, 1.f},
                                    {-0.5f, -0.5f,  0.5f, 1.f}, { 0.5f, -0.5f,  0.5f, 1.f},
                                    { 0.5f,  0.5f,  0.5f, 1.f}, {-0.5f,  0.5f,  0.5f, 1.f} });

    transform(vertices.size(), rotZY, sides);

    indices.insert(indices.end(), { 1,0,2,2,0,3, // order defines normal ? 
                                    4,5,6,6,7,4, // ? only outward part is rendered ? :(
                                    1,2,6,6,5,1,
                                    0,4,3,3,4,7,
                                    0,1,5,5,4,0,
                                    6,2,7,7,2,3 });
};

geometric_shape::geometric_shape(const std::string t, glm::vec3 c, glm::vec2 rotZY) : id(0), type(t), center(c), rotationZY(rotZY) {}

void geometric_shape::transform(int numVerts, glm::vec2 rotationZYAngles, glm::vec3 sides) {

    /*glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    std::cout << glm::to_string(ViewTranslate) << std::endl;
    glm::vec4 res = ViewTranslate * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    std::cout << glm::to_string(res) << std::endl;*/
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::rotate(transform, rotationZYAngles.x, glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::scale(transform, sides);
    transform = glm::translate(transform, center);
    
    //rotate = glm::rotate(rotate, rotationZYAngles.x, glm::vec3(0.0f, 1.0f, 0.0f));      // Yaw
    //rotate = glm::rotate(rotate, rotationZYAngles.y, glm::vec3(1.0f, 0.0f, 0.0f));      // Pitch
    //glm::mat4 rotate = glm::yawPitchRoll(rotationZYAngles.x, rotationZYAngles.y, glm::radians(0.0f));
    //auto rotate = glm::eulerAngleZY(rotationZYAngles.x, rotationZYAngles.y);

    for (size_t i = 0; i < vertices.size(); i++) {
        // set colors to [0,1] range
        colors.push_back(glm::vec3(vertices[i].x, vertices[i].y, vertices[i].z) + glm::vec3(0.5f, 0.5f, 0.5f));
        // rotate using eulerian X->Z angles (while prism is centeted), rescale and translate;
        vertices[i] =  transform * vertices[i];
    }
};
geometric_shape::geometric_shape():id(0) {};
//Prism prism(glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), glm::vec2(0, 0));
//Cube cube(1., glm::vec3(0, 0, 0), glm::vec2(0, 0));
//std::cout << cube.type;
//std::vector<std::unique_ptr<geometric_shape>> scene;
//scene.push_back(std::make_unique<Prism>(glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), glm::vec2(0, 0)));
//scene.push_back(std::make_unique<Cube>(1., glm::vec3(0, 0, 0), glm::vec2(0, 0)));
//auto prism_type = scene[0] -> type;*/



