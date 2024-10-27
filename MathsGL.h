#pragma once
//#include <array>
#include <vector>
#include <glm/glm.hpp>


class MathGL {

public:
	static std::vector<float> matrixVectorMultiply_Nby3(std::vector<glm::vec3> matrix, glm::vec3 vector);
	static std::vector<float> calculateSquaredDistanceUpperTriangleMatrix(std::vector<glm::vec3> coordinates);

};