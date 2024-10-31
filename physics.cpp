#include "physics.h"

std::vector<float> Particles::getParticleGroupsIDs()
{
	std::vector<float> ids;
	ids.reserve(PARTICLE_COUNT);
	uint32_t group_size = PARTICLE_COUNT / PARTICLE_GROUPS;

	// fill full groups UP TO last group
	for (uint32_t i = 0; i < PARTICLE_GROUPS - 1; i++) {
		ids.insert(ids.end(), group_size, i); // Insert group_size instances of i
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

glm::vec3 Physics::rollSphereCoords(float r_min, float r_max, glm::vec3 rolls) {
	std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

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

//std::vector<point3D> Physics::generateParticles(glm::vec3 mass_center_pos, glm::float32 bigMass, glm::float32 grav_const, glm::vec3 reference_axis) {
//	std::default_random_engine rndEngine((unsigned)time(nullptr));
//	std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);
//	std::uniform_real_distribution<float> rndDistClr(0.1f, 0.9f);
//
//	std::vector<point3D> particles(PARTICLE_COUNT);
//	int num_groups = 7;
//	float group_particle_cnt = static_cast<float>(PARTICLE_COUNT) / num_groups;
//	uint32_t goup_members_cnt = 0;
//
//	float r_min = 0.3f;
//	float r_max = 3.5f;
//	glm::vec3 rolls(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine));
//	glm::vec3 pos = rollSphereCoords(r_min, r_max, rolls);
//	glm::vec4 color(rndDistClr(rndEngine), rndDistClr(rndEngine), rndDistClr(rndEngine), 1.0f);
//	for (auto& particle : particles) {
//
//		if (goup_members_cnt > group_particle_cnt - 1) {
//			rolls = glm::vec3(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine));
//			pos = rollSphereCoords(r_min, r_max, rolls);
//			color = glm::vec4(rndDistClr(rndEngine), rndDistClr(rndEngine), rndDistClr(rndEngine), 1.0f);
//			goup_members_cnt = 0;
//		}
//		else {
//			goup_members_cnt++;
//		}
//
//		particle.color = color + glm::vec4(glm::ballRand(0.1f), 0.0f);
//		particle.position = mass_center_pos + pos + glm::ballRand((rndDist(rndEngine) * 0.03f));// glm::vec3(glm::circularRand(0.5f), 0.0f); //glm::vec3(glm::diskRand(0.1f), 0.2f); // ;//  //glm::vec3(0.0f * rndDist(rndEngine), 0.0f * rndDist(rndEngine), 0.0f * 0.2f*rndDist(rndEngine));
//
//		particle.velocity = set_circular_orbit_velocity(mass_center_pos, bigMass, particle.position, grav_const, reference_axis);
//		particle.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
//		particle.mass = glm::float32(1.0);
//		particle.damping = glm::float32(0.0);
//
//
//	}
//
//	return particles;
//}

//std::vector<point3D> Physics::generateParticles2() {
//	// Initialize particles
//	std::default_random_engine rndEngine((unsigned)time(nullptr));
//	std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);
//
//	std::vector<point3D> particles(PARTICLE_COUNT);
//	for (auto& particle : particles) {
//		float r = 0.25f;// *sqrt(rndDist(rndEngine));
//		float theta = rndDist(rndEngine) * 2.0f * 3.14159265358979323846f;
//		float x = r * cos(theta);
//		float y = r * sin(theta);
//		float z = 0.1f;
//		particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
//		particle.position = glm::vec3(x - 0.5f, y, z);//+ glm::ballRand(0.05f);//
//		particle.velocity = glm::ballRand(0.1f);
//		particle.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);		// glm::ballRand(1.0f);// 
//		particle.mass = glm::float32(1.0f);
//		particle.damping = glm::float32(0.0f);
//	}
//
//	return particles;
//}
