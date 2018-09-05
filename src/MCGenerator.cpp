#include "include/MCGenerator.h"

#ifndef MATH_PI
#define MATH_PI 3.14159265359f
#endif

MCGenerator::MCGenerator(generatorType _generator)
	:generator(_generator)
{
	if (generator == generatorType::PYTHIA8) {
		InitializePythia8();
		isPythia8Initialized = true;
	}
}

MCGenerator::MCGenerator() {

}

MCGenerator::~MCGenerator() {
	if (isPythia8Initialized) {
		delete thisPythia;
	}
}

void MCGenerator::NewEvent(std::vector< Track >& tracks) {
	if(generator == generatorType::ISOTROPIC) GetBasicIsotropicEvent(tracks);
	if(generator == generatorType::PYTHIA8) {
		if (!isPythia8Initialized) {
			std::cout << "Warning! Pythia 8.1 was not initalized!  Initializing now, but with default initialization settings!" << std::endl;
			InitializePythia8();
		}
		GetPythia8Event(tracks);
	}
}

//Isotropic generator calls
void MCGenerator::GetBasicIsotropicEvent(std::vector< Track >& tracks) {
	srand(time(NULL));
	dz = std::normal_distribution<double> (0.0, 1.0);
	double thisDz = dz(gausGen);
	double thisDt = dz(gausGen);

	unsigned int multiplicity = (unsigned int)(2 + rand() % 5000);
	for (unsigned int i = 0; i < multiplicity; i++) {
		//random charge of -1 or +1
		int charge = -1 + 2 * (rand() % 2);

		int massRand = (rand() % 100);

		float mass = 0.139f;//pion
		int pid = charge * 211;
		if (massRand < 1) { //muon
			mass = 0.105f;
			pid = -charge * 13;
		}
		else if (massRand < 2) { //electron
			mass = 0.0005f;
			pid = -charge * 11;
		}
		else if (massRand < 10) {//proton
			mass = 0.938f;
			pid = charge * 2212;
		}
		else if (massRand < 20) {//kaon
			mass = 0.494f;
			pid = charge * 321;
		}

		//random momentum
		float pMag = UniformRandom(0.3f, 3.0f);
		float phi = UniformRandom(0.0f, 2 * MATH_PI);
		Track t = Track(UniformRandom(-1, 1), pMag*sin(phi), pMag*cos(phi), charge, mass, pid, (float)thisDz, (float) thisDt);
		tracks.push_back(t);
	}
}

float MCGenerator::UniformRandom(float min, float max) {
	return min + (max - min) * (rand() % 10000) / 10000.0f;
}


//Pythia 8 calls
void MCGenerator::InitializePythia8() {
	//initialize at 13 TeV with proton beams for now
	std::cout << "STARTING PYTHIA 8 INITIALIZATION" << std::endl;
	if (!isPythia8Initialized) {
		thisPythia = new Pythia8::Pythia("resources/PythiaXMLs/xmldoc", false);
		thisPythia->readString("Random:seed = 0");

		//e+e-
		/*
		thisPythia->readString("Beams:idA = 11");
		thisPythia->readString("Beams:idB = -11");
		thisPythia->readString("Beams:eCM = 2000.");
		thisPythia->readString("WeakSingleBoson:all = on");
		thisPythia->readString("WeakDoubleBoson:all = on");*/

		//pp
		thisPythia->readString("Beams:eCM = 13000.");
		thisPythia->readString("PartonLevel:MPI = off");//TURNING THIS ON MAKES GENERATING THINGS TAKE A VERY LONG TIME
		//thisPythia->readString("SoftQCD:inelastic = on");//takes ages to initialize
		thisPythia->readString("SoftQCD:nondiffractive = on");
		//thisPythia->readString("HardQCD:all = on");
		//thisPythia->readString("PhaseSpace:pTHatMin = 300.");
		//thisPythia->readString("Top:all = on");

		if (isPythiaQuiet) thisPythia->readString("Print:quiet = on");
		else thisPythia->readString("Print:quiet = off");

		//always turn these off, as their debugging use is fairly small
		thisPythia->readString("Init:showChangedParticleData  = 0");
		thisPythia->readString("Next:numberShowEvent = 0");
		thisPythia->readString("Next:numberShowProcess = 0");

		thisPythia->init();
	}
	isPythia8Initialized = true;
	std::cout << "PYTHIA IS INITIALIZED" << std::endl;
}

void MCGenerator::GetPythia8Event(std::vector< Track >& tracks) {
	//std::cout << "GETTING AN EVENT FROM PYTHIA" << std::endl;
	dz = std::normal_distribution<double>(0.0, 1.0);
	double thisDz = dz(gausGen);
	double thisDt = dz(gausGen);

	//look for a good event
	while (!thisPythia->next()) {};

	for (int i = 0; i < thisPythia->event.size(); ++i){
		if (thisPythia->event[i].isFinal()) {
			if (thisPythia->event[i].isCharged()) {
				//have to swap the directions b/c pythia's coordinates are not the same as the screen's!
				Track t = Track(thisPythia->event[i].pz(), thisPythia->event[i].py(), thisPythia->event[i].px(),
					thisPythia->event[i].charge(), (float) thisPythia->event[i].m0(), (int) thisPythia->event[i].id(), (float)thisDz, (float)thisDt);
				tracks.push_back( t );
			}
		}
	}
}