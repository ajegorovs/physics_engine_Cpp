#include "physics.h"
#include "lbvh.h"
#include "misc.h"
#include "enable_stuff.h"

std::vector<float> Particles::getParticleGroupsIDs()
{
	std::vector<float> ids;
	ids.reserve(PARTICLE_COUNT);
	uint32_t group_size = PARTICLE_COUNT / PARTICLE_GROUPS;

	// fill full groups UP TO last group
	for (uint32_t i = 0; i < PARTICLE_GROUPS - 1; i++) {
		ids.insert(ids.end(), group_size, static_cast<float>(i)); // Insert group_size instances of i
	}
	// fill last group with extras
	uint32_t missing_elems_num = PARTICLE_COUNT - static_cast<uint32_t>(ids.size());
	ids.insert(ids.end(), missing_elems_num, PARTICLE_GROUPS - 1);
	return ids;
}
	
glm::vec3 Physics::set_circular_orbit_velocity(glm::vec3 mass_center_pos, glm::float32 bigMass, glm::vec3 particle_pos, glm::float32 grav_const, glm::vec3 reference_axis) {
	glm::vec3 d = mass_center_pos - particle_pos;
	glm::float32 dist = glm::length(d);
	glm::vec3 vel_dir = glm::normalize(glm::cross(d, reference_axis));
	//std::cout << glm::dot(vel_dir,d)<<std::endl;
	glm::vec3 orbital_vel = vel_dir * glm::sqrt(bigMass * grav_const / (dist + 0.000001f));

	return orbital_vel;
}