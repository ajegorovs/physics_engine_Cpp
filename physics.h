#pragma once
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <iostream>
#include <random> 
#include "structs.h"

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

	static std::vector<point3D> generateParticles(glm::vec3 mass_center_pos, glm::float32 bigMass, glm::float32 grav_const, glm::vec3 reference_axis) {
		std::default_random_engine rndEngine((unsigned)time(nullptr));
		std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

		std::vector<point3D> particles(PARTICLE_COUNT);
		for (auto& particle : particles) {
			particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 0.5f * rndDist(rndEngine));
			particle.position = mass_center_pos + glm::vec3(0.0f, 2.0f, 0.0f) + glm::ballRand((rndDist(rndEngine) * 0.03f));// glm::vec3(glm::circularRand(0.5f), 0.0f); //glm::vec3(glm::diskRand(0.1f), 0.2f); // ;//  //glm::vec3(0.0f * rndDist(rndEngine), 0.0f * rndDist(rndEngine), 0.0f * 0.2f*rndDist(rndEngine));

			particle.velocity = set_circular_orbit_velocity(mass_center_pos, bigMass, particle.position, grav_const, reference_axis);// glm::ballRand(1.0f);// 
			particle.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
			particle.mass = glm::float32(1.0);
			particle.damping = glm::float32(0.0);
		}

		return particles;
	}

	static std::vector<point3D> generateParticles2() {
		// Initialize particles
		std::default_random_engine rndEngine((unsigned)time(nullptr));
		std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

		std::vector<point3D> particles(PARTICLE_COUNT);
		for (auto& particle : particles) {
			float r = 0.25f;// *sqrt(rndDist(rndEngine));
			float theta = rndDist(rndEngine) * 2.0f * 3.14159265358979323846f;
			float x = r * cos(theta);
			float y = r * sin(theta);
			float z = 0.1f;
			particle.color			= glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
			particle.position		= glm::vec3(x - 0.5f, y, z);//+ glm::ballRand(0.05f);//
			particle.velocity		= glm::ballRand(0.1f);
			particle.acceleration	= glm::vec3(0.0f, 0.0f, 0.0f);		// glm::ballRand(1.0f);// 
			particle.mass			= glm::float32(1.0f);
			particle.damping		= glm::float32(0.0f);
		}

		return particles;
	}
};