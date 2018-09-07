#include "include/MyEvent.h"
#define MAX_EVENTS_BEFORE_USING_COPIES 300
#define SIZE_OF_EVENT_VTXBUFFER_BYTES 250000000
#define SHORTS_IN_VTX_BUFFER 5
#define SPEED_OF_LIGHT_M_PER_NS 0.2998f
#define UNIT_CONVESTION_FOR_P 1.602f*SPEED_OF_LIGHT_M_PER_NS/5.39f
#define SQRT2 1.4142135f
#define TURN_OFF_MULTITHREADING 0 //0 = multithreading turned on, 1 = no multithreading


MyEvent::MyEvent(double _animationTime, double _fadeStartTime, Beam* _beam, MCGenerator* _mcGen)
{
	thisMCGenerator = _mcGen;
	beamline = _beam;
	trackLengthModifier = beamline->GetSecondsToNSConversion();
	delayTime = beamline->GetBunchCrossingDelay() / beamline->GetSecondsToNSConversion();
	refreshTime = ( beamline->GetBunchSpacing() )/beamline->GetSecondsToNSConversion() - delayTime;
	animationTime = _animationTime * refreshTime;
	fadeStartTime = _fadeStartTime * refreshTime;

	nTimeIntervals = (unsigned short)( 60.0 * animationTime ); //60 because we want 60 points per second
	timeStep = (float) trackLengthModifier / 60.f; //each timestep is 1/60th of a ns (1s -> 1 ns) if modifier is 1
	
	trackBufferLayout.Push<short>(3);
	trackBufferLayout.Push<unsigned short>(1);
	trackBufferLayout.Push<short>(1);

	trkVertexBuffer = new VertexBuffer();
	trkVertexBuffer->AddData((const void*)NULL, SIZE_OF_EVENT_VTXBUFFER_BYTES);
	trkVertexArray = new VertexArray();

	//launch new thread to get an event
	mutex_writingToSimulationOutput = new std::mutex();
	eventGeneratorThreads.push_back(std::thread(&MyEvent::FillEventBuffer, this));//fill event buffer

	//get ready to setup
	ResetSetupBooleans();
	isVertexAttributeObjectReady = false;//only setup one time
	
	//swap in new event and start setup process
	GetNewEvent();
	SetupDraw();
}

MyEvent::~MyEvent() {
	DeleteVertexBuffers();
	DeleteVertexArrays();
}

int MyEvent::Update() {
	timeSinceEventStart = beamline->GetTimeSinceLastBunchCrossing() - delayTime;

	if ( beamline->GetBPTXFlag() ) {//get new event if there has been a bunch crossing
		beamline->SetBPTXFlag(false); //reset the flag to acknowledge that we got the message from the beamline
		GetNewEvent();

		ResetSetupBooleans();
		SetupDraw();
		return 2;
	}

	if (isSettingUpNextEvent) {//keep setting up next event if we are still doing that
		SetupDraw();
		return 1;
	}

	return 0;//otherwise return 0
}

void MyEvent::ResetSetupBooleans() {
	isSettingUpNextEvent = true;
	isVertexBufferReady = false;
}

//called multiple times while isSettingUpNextEvent is true
//keeps setting up buffers in stages until it is ready
void MyEvent::SetupDraw() {
	//setup VAO 1 time
	if (!isVertexAttributeObjectReady) {
		trkVertexArray->AddBuffer(*trkVertexBuffer, trackBufferLayout);
		isVertexAttributeObjectReady = true;
		return;
	}

	//Vertex buffer
	if ( beamline->GetTimeSinceLastBunchCrossing() > delayTime) {
		std::cout << "nTracks: " << nTrack[renderIndex] << std::endl;

		Timer time = Timer();
		trkVertexBuffer->SubData(vtxPositions[renderIndex].data(), vtxPositions[renderIndex].size() * sizeof(short), 0);
		glFinish();
		std::cout << "Vtx Buffer took " << time.TimeSplit() << " s to create" << std::endl;

		isSettingUpNextEvent = false;
	}
}

void MyEvent::Draw(Renderer* r, Shader* s) {
	GLCall(glLineWidth(2));
	s->Bind();
	s->SetUniform1f("u_fractionOfPropagationTimeElapsed", (float)(timeSinceEventStart) / (float)(animationTime));
	s->SetUniform1f("u_minAlpha", 0.5f);
	s->SetUniform1f("u_pointsOnTrack", (float)nTimeIntervals);
	s->SetUniform1f("u_nPointsOfGradient", 0.025f * nTimeIntervals);
	s->SetUniform1f("u_alphaModifierForFade", getAlphaModiferForFade());
	s->SetUniform1f("u_inverseLengthScale", (float) 1.0/lengthScale[renderIndex]);

	s->SetUniform4f("u_ColorPion", 0.1f, 0.8f, 0.1f, 0.0f);     //pion
	s->SetUniform4f("u_ColorKaon", 0.6f, 0.2f, 1.0f, 0.0f);     //kaon
	s->SetUniform4f("u_ColorProton", 0.0f, 0.8f, 1.0f, 0.0f);   //proton
	s->SetUniform4f("u_ColorElectron", 1.0f, 1.0f, 0.2f, 0.0f); //electron
	s->SetUniform4f("u_ColorMuon", 0.8f, 0.05f, 0.05f, 0.0f);   //muon
	s->SetUniform4f("u_ColorDefault", 1.0f, 1.0f, 1.0f, 0.0f);  //other

	//curtail number of points to draw if we aren't at the time limit
	std::vector< GLsizei > tempPointsOnTrack = pointsOnTrack[renderIndex];
	unsigned int maxIndicesToDraw = 1 + (unsigned int)(nTimeIntervals * (timeSinceEventStart) / animationTime);
	for (unsigned int i = 0; i < tempPointsOnTrack.size(); i++) {
		if (maxIndicesToDraw < (unsigned int)(tempPointsOnTrack[i])) tempPointsOnTrack[i] = maxIndicesToDraw;
	}

	r->MultiDrawNoIB(*(trkVertexArray), *s, GL_LINE_STRIP, tempPointsOnTrack.data(), (GLsizei*)trackOffsets[renderIndex].data(), nTrack[renderIndex]);
}

float MyEvent::getAlphaModiferForFade() {
	if (timeSinceEventStart < fadeStartTime) return 1.0f;
	if (timeSinceEventStart > refreshTime) return 0.0f;
	return (float)(1.0f - (timeSinceEventStart - fadeStartTime) / (refreshTime - fadeStartTime));
}

void MyEvent::DeleteVertexBuffers() {
	delete trkVertexBuffer;
}

void MyEvent::DeleteVertexArrays() {
	delete trkVertexArray;
}


void MyEvent::GetNewEvent() {
	(eventGeneratorThreads.at(0)).join();//wait for the event buffer thread to finish before swapping buffer with existing data

	//swap out buffer items for 'active' items being drawn
	generatorIndex = renderIndex;
	renderIndex = (generatorIndex + 1) % 2;

	timeStep = (float)trackLengthModifier / 60.f; //each timestep is 1/60th of a ns (1s -> 1 ns)

	//empty thread container and launch a new thread to fill up the (now empty) event buffer
	std::vector< std::thread >().swap(eventGeneratorThreads);
	eventGeneratorThreads.push_back(std::thread(&MyEvent::FillEventBuffer, this));//fill event buffer
}

void MyEvent::FillEventBuffer() {
	Timer time = Timer();

	//free vectors memory
	std::vector<Track>().swap(tracks[generatorIndex]);
	std::vector< GLsizei >().swap(pointsOnTrack[generatorIndex]);
	std::vector< unsigned int >().swap(trackOffsets[generatorIndex]);
	std::vector< short >().swap(vtxPositions[generatorIndex]);
	std::cout << "Clearing Buffers took: " << time.Restart() << " s" << std::endl;

	//add pileups if wanted
	std::default_random_engine generator;
	generator.seed((unsigned int)std::time(NULL));
	unsigned int interactions;

	beamline->mutex_pileup->lock();
	std::poisson_distribution<int> poisson(beamline->GetPileup());
	while (true) {
		if (!(beamline->GetIsPoissonPU())) {//fixed pu
			interactions = (unsigned int)(beamline->GetPileup() + 0.001);//add slight offset to take care of any weird float conversion errors
		}
		else {//poisson
			interactions = (unsigned int)(poisson(generator));
		}
		if (interactions > 0 || (!beamline->GetIgnore0PU()) ) break;
	}
	beamline->mutex_pileup->unlock();

	//get a SIGNAL event from our event generator
	std::cout << "Interactions: " << interactions << std::endl;
	if (interactions > 0) {
		thisMCGenerator->NewEvent(tracks[generatorIndex], 1);
		interactions--;
	}

	//start reusing slighly randomized pythia events multiple times once pileup reaches a certain point just to speed things up
	unsigned int nCopies = 1 + (int)(trackLengthModifier*interactions/MAX_EVENTS_BEFORE_USING_COPIES);
	for (unsigned int i = interactions; i > 0 ; ) {
		if (i >= nCopies) {
			thisMCGenerator->NewEvent(tracks[generatorIndex], nCopies);
			i -= nCopies;
		}
		else {
			thisMCGenerator->NewEvent(tracks[generatorIndex], i);
			break;
		}
	}

	//get bunchlength
	beamline->mutex_bunchLength->lock();
	float bunchLength = beamline->GetBunchLength();
	beamline->mutex_bunchLength->unlock();

	//calculate time offset (for shifting events so that the 'first' event is produced at t=0 in the bunch crossing
	float minT = 9999.0;
	for (unsigned int i = 0; i < tracks[generatorIndex].size(); i++) {
		if (tracks[generatorIndex].at(i).dt() < minT) minT = tracks[generatorIndex].at(i).dt();
	}

	nTrack[generatorIndex] = 0;
	std::cout << "Generation took: " << time.Restart() << " s" << std::endl;

	//run simulation on event
	//calculate now many threads we want
	//subtract because we want to leave a thread open for the main program and 1 'spare' thread to not flood the computer
	int nThreads = std::thread::hardware_concurrency()-2;
	if (nThreads <= 0 || TURN_OFF_MULTITHREADING) nThreads = 1;

	//launch extra threads but skip 0 because the launching thread is the 0th thread
	std::vector< std::thread >().swap(eventSimulationThreads);
	void (MyEvent::*thisRunSimulation)(int, int, float, float) = &MyEvent::RunSimulation;//this selects the correct overload of RunSimulation (4 args)
	for (int i = 1; i < nThreads; i++) {
		eventSimulationThreads.push_back( std::thread( thisRunSimulation, this, i, nThreads, minT, bunchLength ) );
	}
	RunSimulation(nThreads, minT, bunchLength);
	//collect remaining threads
	for (unsigned int i = 0; i < eventSimulationThreads.size(); i++) eventSimulationThreads.at(i).join();

	std::cout << "Simulation took: " << time.TimeSplit() << " s for " << nTrack[generatorIndex] << " tracks." << std::endl;
}

void MyEvent::RunSimulation(int nThreads, float minT, float bunchLength) { RunSimulation(0, nThreads, minT, bunchLength); }

void MyEvent::RunSimulation(int threadID, int nThreads, float minT, float bunchLength) {
	//relevant length scale is set by the maximum propagation range of particles during the animation (ctau)
	//we subdivide this space into a grid of (max short) width so that we can save space in the vtx buffer
	//add 3 sigma of the bunchlength because particles can start not at the origin
	float maxSizeOfSignedShort = 32768 - 300;//300 is a 1% safety limit for overflow safety
	lengthScale[generatorIndex] = (float) (maxSizeOfSignedShort / (nTimeIntervals*timeStep*SPEED_OF_LIGHT_M_PER_NS + 3 * SQRT2*bunchLength / 100.0));

	short * tempPositions = (short*) malloc(nTimeIntervals * sizeof(short) * SHORTS_IN_VTX_BUFFER);
	glm::vec3 B = glm::vec3(3.8f, 0.0f, 0.0f);

	for (unsigned int i = 0; i < tracks[generatorIndex].size(); i++) {
		if (i % nThreads != threadID) continue;//have the nth thread do the nth track in a cycle...
		Track t = tracks[generatorIndex][i];

		//apply relevant cuts
		if (doPtCut && t.Pt() < ptCut) continue;
		if (doEtaCut && (t.Eta() < etaCutLow || t.Eta() > etaCutHigh)) continue;

		unsigned int tempPositionsValidLength = 0;

		//setup initial parameters
		glm::vec3 trkX = glm::vec3(t.dz()*(SQRT2*bunchLength / 100.0f), 0.0f, 0.0f);//offset in z direction some
		glm::vec3 trkP = glm::vec3(t.Px(), t.Py(), t.Pz());

		//calculate some convenient variables once before going into loop
		float e = sqrt(t.P2() + t.Mass()*t.Mass());
		float position_EulerCoefficient =  timeStep * SPEED_OF_LIGHT_M_PER_NS / e;
		float momentum_RK4Coefficient =  t.Charge() / e * timeStep * UNIT_CONVESTION_FOR_P;
		float timeOffset =  (t.dt() - minT) * SQRT2*bunchLength / 100.0f / SPEED_OF_LIGHT_M_PER_NS;
		bool isGoodPropagation = true;

		//relativistic particle in B field is same as non-relativistic w/ mass term replaced by gamma*mass (energy)
		for (unsigned short j = 0; j < nTimeIntervals; j++) {
			//if (i>10 && j > 180) continue;  //can terminate tracks early here with a continue

			//write down positions
			for (int k = 0; k < 3; k++)  tempPositions[j*SHORTS_IN_VTX_BUFFER + k] = (short)(trkX[k] * lengthScale[generatorIndex]);
			//std::cout << j << " " << trkX[0] << " " << trkX[1] << " " << trkX[2] << std::endl;
			
			tempPositions[j*SHORTS_IN_VTX_BUFFER + 3] = *(short*)(&j);//convert unsigned to signed in memory
			tempPositions[j*SHORTS_IN_VTX_BUFFER + 4] = (short)abs(t.PID());
			tempPositionsValidLength += SHORTS_IN_VTX_BUFFER;

			//keep particles at their original position/momentum if they haven't been produced yet (from a delayed production vertex)
			if ( timeOffset >= j*timeStep) continue;

			//update positions for next iteration based on the momentum from this iteration
			glm::vec3 newX = glm::vec3();
			for (int k = 0; k < 3; k++) newX[k] = (float)(trkX[k] + position_EulerCoefficient * trkP[k]);

			//RK4 integration of ODE of momentum dp/dt = q(p x B)/E
			//unit conversion is needed b/c B is in Tesla (SI), t is in ns, but p,E,q are in natural units
			glm::vec3 k1 = glm::vec3();
			glm::vec3 pXB = glm::cross(trkP, B);
			for (int k = 0; k < 3; k++) k1[k] = (float)(momentum_RK4Coefficient * pXB[k]);

			glm::vec3 k2 = glm::vec3();
			pXB = glm::cross(trkP + k1 * 0.5f, B);
			for (int k = 0; k < 3; k++) k2[k] = (float)(momentum_RK4Coefficient * pXB[k]);

			glm::vec3 k3 = glm::vec3();
			pXB = glm::cross(trkP + k2 * 0.5f, B);
			for (int k = 0; k < 3; k++) k3[k] = (float)(momentum_RK4Coefficient * pXB[k]);

			glm::vec3 k4 = glm::vec3();
			pXB = glm::cross(trkP + k3, B);
			for (int k = 0; k < 3; k++) k4[k] = (float)(momentum_RK4Coefficient * pXB[k]);

			glm::vec3 newP = glm::vec3();
			for (int k = 0; k < 3; k++) newP[k] = (float)(trkP[k] + (k1[k] + 2 * k2[k] + 2 * k3[k] + k4[k]) / 6.0f);
			
			if ((isGoodPropagation == true) && (glm::length(newP) / e > 1.01f)) {
				std::cout << "Numerical instability detected! Not outputting this track!" << std::endl;
				isGoodPropagation = false;
			}

			trkX = newX;
			trkP = newP;
			//if (i == 0) std::cout <<"\n" << j<< " \n" << trkX[0] << " " << trkX[1] << " " << trkX[2] << std::endl;
			//if (i == 0) std::cout << trkP[0] << " " << trkP[1] << " " << trkP[2] << std::endl;
		}

		//if propagation was good, copy temp vector into master positions vector
		mutex_writingToSimulationOutput->lock();
		if (isGoodPropagation) {
			if (SIZE_OF_EVENT_VTXBUFFER_BYTES / sizeof(short) - vtxPositions[generatorIndex].size() < 	tempPositionsValidLength) {
				std::cout << "WARNING: VTX BUFFER IS NOT BIG ENOUGH TO HANDLE THESE EVENTS!" << std::endl;
				mutex_writingToSimulationOutput->unlock();
				break;
			}
			nTrack[generatorIndex]++;

			unsigned int oldArraySize = vtxPositions[generatorIndex].size();
			trackOffsets[generatorIndex].push_back((GLsizei)(oldArraySize / SHORTS_IN_VTX_BUFFER));
			for (unsigned int i = 0; i < tempPositionsValidLength; i++) {
				vtxPositions[generatorIndex].push_back(tempPositions[i]);
			}
			pointsOnTrack[generatorIndex].push_back((GLsizei) tempPositionsValidLength / SHORTS_IN_VTX_BUFFER);
		}
		mutex_writingToSimulationOutput->unlock();
	}
	free(tempPositions);
}

