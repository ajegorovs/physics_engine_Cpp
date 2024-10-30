#pragma once
#include <glm/glm.hpp>
//#include "glm/ext.hpp"
//#include "<glm/gtc/constants.hpp>"
#include <iostream>
#include <random> 
#include <vector>
#include <array>
#include "structs.h"


const uint32_t PARTICLE_COUNT = 256 * 17145;
const uint32_t PARTICLE_GROUPS = 14;
const float GRAV_CONST = 1;
const uint32_t NUM_ATTRACTORS = 2;

const float BLOB_R_MIN = 0.5f;
const float BLOB_R_MAX = 2.5f;

struct StructAttractor
{
	alignas(4) float mass;
	alignas(4) float radius;
	alignas(4) float density;
	alignas(16) glm::vec3 position;
};

struct StructParticleSystemParams
{
	alignas(4) float		num_attractors;
	alignas(4) glm::float32 grav_const;
	alignas(4) uint32_t		particle_count;
	alignas(4) float		blob_r_min;
	alignas(4) float		blob_r_max;
};

class Physics {

public:
	; static glm::vec3 set_circular_orbit_velocity(glm::vec3 mass_center_pos, glm::float32 bigMass, glm::vec3 particle_pos, glm::float32 grav_const, glm::vec3 reference_axis);

	static glm::vec3 rollSphereCoords(float r_min, float r_max, glm::vec3 rolls);

	//static std::vector<point3D> generateParticles(glm::vec3 mass_center_pos, glm::float32 bigMass, glm::float32 grav_const, glm::vec3 reference_axis);


	//static std::vector<point3D> generateParticles2();
};

class Particles {
public:
	static std::vector<uint32_t> getParticleGroupsIDs();
};