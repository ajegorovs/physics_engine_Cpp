#include "MathsGL.h"
#include <iostream>


std::vector<float> MathGL::matrixVectorMultiply_Nby3(std::vector<glm::vec3> matrix, glm::vec3 vector)
{
    std::vector<float> result(matrix.size());

    for (size_t i = 0; i < matrix.size(); i++) {
        result[i] = glm::dot(matrix[i], vector);
    }

    return result;
}

std::vector<float> MathGL::calculateSquaredDistanceUpperTriangleMatrix(std::vector<glm::vec3> coordinates)
{
    size_t N = coordinates.size();
    std::vector<float> result; 
    result.reserve(N * (N - 1) / 2); // num triangle matrix elements (see derivation in ./images)
    for (size_t i = 0; i < N; i++) {
        for (size_t j = i + 1; j < N ; j++) {
            //std::cout << "(" << i << ", " << j << ")" << std::endl;
            result.push_back(   glm::dot(coordinates[i], coordinates[j])    );
        }
    }

    return result;
}
