#include "include/BFieldMap.h"

BFieldMap::BFieldMap() {
	nThreads = (int) std::thread::hardware_concurrency() - 2;
	if (nThreads < 1) nThreads = 1;
	for (int i = 0; i < nThreads; i++) {
		std::mutex * tempMutex = new std::mutex;
		mutex_BFieldMap.push_back(tempMutex);
	}
}

BFieldMap::~BFieldMap() {
	for (int i = 0; i < nThreads; i++) {
		delete mutex_BFieldMap[i];
	}
}

glm::vec3 BFieldMap::getBField(glm::vec3 r) {
	glm::vec3 B = glm::vec3(0);

	int mutexLockNumber = -1;
	for (int i = 0; i < nThreads; i++) {
		//try to find an unlocked mutex and then proceed
		if (mutex_BFieldMap[i]->try_lock()) {
			mutexLockNumber = i;
			break;
		}

		//if all mutexes are locked, then we just return B=0 for speed
		//this should only happen if everything is locked for addition/removal/editing of magnets
		if (i == nThreads - 1) return B;
	}

	for (size_t i = 0; i < magnets.size(); i++) {
		B += magnets.at(i)->getBField(r);
	}

	//unlock whatever mutex we locked earlier
	mutex_BFieldMap[mutexLockNumber]->unlock();
	return B;
}

void BFieldMap::addMagnet(Solenoid* s) {
	//lock all mutexes
	for (int i = 0; i < nThreads; i++) mutex_BFieldMap[i]->lock();
	magnets.push_back(s);
	//unlock all mutexes
	for (int i = 0; i < nThreads; i++) mutex_BFieldMap[i]->unlock();
}

void BFieldMap::removeMagnet(int i) {
	//lock all mutexes
	for (int i = 0; i < nThreads; i++) mutex_BFieldMap[i]->lock();
	magnets.erase(magnets.begin()+i);
	//unlock all mutexes
	for (int i = 0; i < nThreads; i++) mutex_BFieldMap[i]->unlock();
}