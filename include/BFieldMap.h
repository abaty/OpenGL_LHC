#pragma once
#include "glm/glm.hpp"
#include "include/Solenoid.h"
#include <vector>
#include <mutex>

class BFieldMap {
public:
	BFieldMap();
	~BFieldMap();

	glm::vec3 getBField(glm::vec3 r);
	void addMagnet( Solenoid *s);
	void removeMagnet(int i);

	std::vector < std::mutex * > mutex_BFieldMap;
    int nThreads;

private:
	std::vector< Solenoid* > magnets;
	size_t magVecSize = 0;
};