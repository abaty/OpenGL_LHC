#pragma once
#include "Pythia8/Pythia.h"
#include <include/Track.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <random>

enum generatorType {
	ISOTROPIC, PYTHIA8
};

class MCGenerator {
public:
	MCGenerator();
	MCGenerator(generatorType _generator);
	~MCGenerator();

	void SetCurrentGenerator(generatorType g) { generator = g; }
	void SetIsPythiaQuiet(bool _b) { isPythia8Initialized = _b; }

	void NewEvent(std::vector< Track >& tracks, unsigned short nCopies);

private:
	
	generatorType generator = generatorType::ISOTROPIC;

	//isotropic generator
	void GetBasicIsotropicEvent(std::vector< Track >& tracks);
	float UniformRandom(float min, float max);

	//pythia 8

	Pythia8::Pythia* thisPythia;
	bool isPythia8Initialized = false;
	void InitializePythia8();
	void GetPythia8Event(std::vector< Track >& tracks, unsigned short nCopies);
	bool isPythiaQuiet = true;


	std::default_random_engine gausGen;
	std::normal_distribution<double> dz;
};
