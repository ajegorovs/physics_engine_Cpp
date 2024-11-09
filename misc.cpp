#include "misc.h"
#include "lbvh.h"
#include "config.h"
#include <fstream>  
//#include <random>  
//#include <array>  
//#include <algorithm> 
#include <sstream>
//#include <iostream>


std::vector<char> Misc::readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkShaderModule Misc::createShaderModule(VkDevice* pDevice, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(*pDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

unsigned int Misc::expandBits(unsigned int v)
{
    v = (v * 0x00010001u) & 0xFF0000FFu;
    v = (v * 0x00000101u) & 0x0F00F00Fu;
    v = (v * 0x00000011u) & 0xC30C30C3u;
    v = (v * 0x00000005u) & 0x49249249u;
    return v;
}

unsigned int Misc::morton3D(float x, float y, float z)
{
    x = std::min(std::max(x * 1024.0f, 0.0f), 1023.0f);
    y = std::min(std::max(y * 1024.0f, 0.0f), 1023.0f);
    z = std::min(std::max(z * 1024.0f, 0.0f), 1023.0f);
    unsigned int xx = expandBits((unsigned int)x);
    unsigned int yy = expandBits((unsigned int)y);
    unsigned int zz = expandBits((unsigned int)z);
    return xx * 4 + yy * 2 + zz;
}

glm::vec3 Misc::rollSphereCoords(float r_min, float r_max, glm::vec3 rolls) {

    float pi = glm::pi<float>();
    float r = (r_max - r_min) * rolls[0] + r_min;
    float theta = pi * rolls[1];
    float phi = 2 * pi * rolls[2];

    return glm::vec3(
        r * glm::sin(theta) * glm::cos(phi),
        r * glm::sin(theta) * glm::sin(phi),
        r * glm::cos(theta)
    );
}

std::vector<glm::vec3> Misc::seedUniformPoints2D(const int N)
{
    std::vector<glm::vec3> output;
    output.reserve(N);
    std::default_random_engine rndEngine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

    for (size_t i = 0; i < N; i++)
    {
        glm::vec3 p{ rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine) };
        output.push_back(p);
    }
    return output;
}

std::vector<glm::vec3> Misc::seedUniformGridPoints3D(const int N) {
    std::vector<glm::vec3> points;
    points.reserve(N);
    // Compute the approximate number of divisions per dimension
    int numDivisions = static_cast<int>(std::floor(std::cbrt(N)));
    if (numDivisions * numDivisions * numDivisions < N) {
        numDivisions++;
    }

    float spacing = 1.0f / (numDivisions - 1); // Spacing between points in each dimension

    for (int x = 0; x < numDivisions; ++x) {
        for (int y = 0; y < numDivisions; ++y) {
            for (int z = 0; z < numDivisions; ++z) {
                if (points.size() < static_cast<size_t>(N)) {  // Only add up to N points
                    float px = x * spacing;
                    float py = y * spacing;
                    float pz = z * spacing;
                    points.emplace_back(px, py, pz);
                }
            }
        }
    }

    return points;
}

std::vector<glm::vec3> Misc::seedUniformSpherePoints3D(const int N)
{
    std::vector<glm::vec3> points;
    points.reserve(N);
    std::default_random_engine rndEngine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);
    for (size_t i = 0; i < N; i++)
    {
        float a = rndDist(rndEngine);
        float b = rndDist(rndEngine);
        float c = rndDist(rndEngine);
        glm::vec3 p = rollSphereCoords(0.3f, 0.5f, glm::vec3(a, b, c));
        p += glm::vec3(0.5f, 0.5f, 0.5f);
        //std::cout << p[0] << " " << p[1] << " " << p[2] << std::endl;
        points.push_back(p);
    }
    return points;
}


// Assume Misc::morton3D function is defined elsewhere

std::vector<glm::vec3> Misc::sortByMorton(std::vector<glm::vec3> input)
{
    // Generate Morton code for each point and store them with their associated points
    std::vector<std::pair<unsigned int, glm::vec3>> mortonPairs;
    mortonPairs.reserve(input.size());

    for (const auto& p : input)
    {
        mortonPairs.emplace_back(Misc::morton3D(p[0], p[1], p[2]), p);
    }

    // Sort the pairs by Morton code
    std::sort(mortonPairs.begin(), mortonPairs.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    // Extract sorted points
    std::vector<glm::vec3> sortedInput;
    sortedInput.reserve(input.size());

    for (const auto& pair : mortonPairs)
    {
        sortedInput.push_back(pair.second);
    }

    return sortedInput;
}

std::vector<float> Misc::getExtent(std::vector<glm::vec3> points)
{
    glm::vec3 minV = points[0];
    glm::vec3 maxV = points[0];

    for (glm::vec3 p : points)
    {
        minV = glm::min(minV, p);
        maxV = glm::max(maxV, p);
    }
    minV -= glm::vec3(P_R);
    maxV += glm::vec3(P_R);
    std::vector<float> output;
    output.reserve(6);
    output.insert(output.end(), &minV[0], &minV[0] + 3);
    output.insert(output.end(), &maxV[0], &maxV[0] + 3);
   /* output.push_back(minV[0]);
    output.push_back(minV[1]);
    output.push_back(minV[2]);
    output.push_back(maxV[0]);
    output.push_back(maxV[1]);
    output.push_back(maxV[2]);*/

    return output;
}


std::vector<MortonCodeElement> Misc::importFromCSV(const std::string& filename) {
    std::vector<MortonCodeElement> elements;
    std::ifstream myfile(filename);

    if (!myfile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return elements; // Return an empty vector
    }

    std::string line;
    bool skipHeader = true;

    while (std::getline(myfile, line)) {
        if (skipHeader) {
            skipHeader = false;  // Skip the header line
            continue;
        }

        std::istringstream ss(line);
        MortonCodeElement element;
        std::string value;

        // Read the mortonCode
        if (std::getline(ss, value, ',')) {
            element.mortonCode = std::stoul(value);
        }

        // Read the elementIdx
        if (std::getline(ss, value, ',')) {
            element.elementIdx = std::stoul(value);
        }

        elements.push_back(element);
    }

    myfile.close();
}