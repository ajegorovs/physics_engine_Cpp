#pragma once
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <iostream>

class Physics {

public:
	static glm::vec3 set_circular_orbit_velocity(glm::vec3 mass_center_pos, glm::float32 bigMass, glm::vec3 particle_pos, glm::float32 grav_const, glm::vec3 reference_axis) {
		glm::vec3 d = mass_center_pos - particle_pos;
		glm::float32 dist = glm::length(d);
		glm::vec3 vel_dir = glm::normalize(glm::cross(d, reference_axis));
		//std::cout << glm::dot(vel_dir,d)<<std::endl;
		glm::vec3 orbital_vel = vel_dir * glm::sqrt(bigMass * grav_const / (dist + 0.000001f));

		return orbital_vel;
	}
};