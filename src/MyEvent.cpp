#include "include/MyEvent.h"
#define SPEED_OF_LIGHT_M_PER_NS 0.2998
#define UNIT_CONVESTION_FOR_P 1.602*SPEED_OF_LIGHT_M_PER_NS/5.39
#define SQRT2 1.4142135

MyEvent::MyEvent(double _animationTime, double _fadeStartTime, Beam* _beam, MCGenerator* _mcGen)
	: timeSinceStart(0)
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

	//with index buffer
	//trkIB.AddData((const unsigned int*)NULL, 100000000);
	trkVertexBuffer = new VertexBuffer();
	trkVertexBuffer->AddData((const void*)NULL, 100000000);
	trkVertexArray = new VertexArray();

	//launch new thread to get an event
	eventGeneratorThreads.push_back(std::thread(&MyEvent::FillEventBuffer, this));//fill event buffer

	//get ready to setup
	ResetSetupBooleans();
	isVertexAttributeObjectReady = false;//only setup one time
	
	//swap in new event and start setup process
	GetNewEvent();
	startTime = glfwGetTime();
	SetupDraw();
}

MyEvent::~MyEvent() {
	DeleteVertexBuffers();
	DeleteVertexArrays();
}

int MyEvent::Update() {
	timeSinceStart = glfwGetTime() - startTime;
	timeSinceEventStart = timeSinceStart - delayTime;
	if (timeSinceEventStart > refreshTime) {//get new event if it's been too long
		GetNewEvent();

		startTime = glfwGetTime();
		timeSinceStart = 0;
		timeSinceEventStart = -delayTime;

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
	//with index buffer
	//isIndexBufferReady = false;
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

	/*
	//bind index arrays
	if (!isIndexBufferReady) {
		Timer time = Timer();
		trkIB.AddData(indexArray[renderIndex].data(), indexArray[renderIndex].size());
		glFinish();
		std::cout << "Index Array took " << time.Restart() << " s to bind" << std::endl;
		
		isIndexBufferReady = true;
		return;
	}*/

	//Vertex buffer
	if (timeSinceStart > delayTime) {
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
	s->SetUniform1f("u_fractionOfPropagationTimeElapsed", (float)(timeSinceEventStart) / (float)(animationTime));
	s->SetUniform1f("u_minAlpha", 0.5f);
	s->SetUniform1f("u_pointsOnTrack", (float)nTimeIntervals);
	s->SetUniform1f("u_nPointsOfGradient", 0.025f * nTimeIntervals);
	s->SetUniform1f("u_alphaModifierForFade", getAlphaModiferForFade());
	s->SetUniform1f("u_inverseLengthScale", 1.0/lengthScale[renderIndex]);

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

	//with indexBuffer
	//r->MultiDraw(*(trkVertexArray), trkIB, *s, GL_LINE_STRIP, tempPointsOnTrack.data(), trackOffsets[renderIndex].data(), nTrack[renderIndex]);
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
	//free vectors memory

	//with index buffer
	//std::vector< unsigned int >().swap(indexArray[generatorIndex]);
	std::vector<Track>().swap(tracks[generatorIndex]);
	std::vector< short >().swap(vtxPositions[generatorIndex]);
	std::vector< GLsizei >().swap(pointsOnTrack[generatorIndex]);
	std::vector< unsigned int >().swap(trackOffsets[generatorIndex]);

	//get a SIGNAL event from our event generator
	thisMCGenerator->NewEvent(tracks[generatorIndex]);
	//add pileups if wanted
	beamline->mutex_pileup->lock();
	unsigned int extraInteractions = (unsigned int)(beamline->GetPileup() + 0.001 - 1);//add slight offset to take care of any weird float conversion errors
	beamline->mutex_pileup->unlock();
	for (unsigned int i = 0; i < extraInteractions; i++) {
		thisMCGenerator->NewEvent(tracks[generatorIndex]);
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

	//run simulation on event
	Timer time = Timer();
	for (unsigned int i = 0; i < tracks[generatorIndex].size(); i++) {
		Track t = tracks[generatorIndex].at(i);

		//apply relevant cuts
		if (doPtCut && t.Pt() < ptCut) continue;
		if (doEtaCut && (t.Eta() < etaCutLow || t.Eta() > etaCutHigh)) continue;

		std::vector< float > tempPositions;
		//relevant length scale is set by the maximum propagation range of particles during the animation (ctau)
		//we subdivide this space into a grid of (max short) width so that we can save space in the vtx buffer
		//add 3 sigma of the bunchlength because particles can start not at the origin
		float maxSizeOfSignedShort = 32768-300;//300 is a 1% safety limit for overflow safety
		lengthScale[generatorIndex] = maxSizeOfSignedShort /(nTimeIntervals*timeStep*SPEED_OF_LIGHT_M_PER_NS + 3*SQRT2*bunchLength/100.0);

		//setup initial parameters
		glm::vec3 trkX = glm::vec3(t.dz()*(SQRT2*bunchLength / 100.0f), 0.0f, 0.0f);//offset in z direction some
		glm::vec3 trkP = glm::vec3(t.Px(), t.Py(), t.Pz());
		glm::vec3 B = glm::vec3(3.8f, 0.0f, 0.0f);

		double e = sqrt(t.P2() + t.Mass()*t.Mass());
		bool isGoodPropagation = true;

		//relativistic particle in B field is same as non-relativistic w/ mass term replaced by gamma*mass (energy)
		for (unsigned short j = 0; j < nTimeIntervals; j++) {
			//if (i>10 && j > 180) continue;  //can terminate tracks early here with a continue

			//write down positions
			for (int k = 0; k < 3; k++)  tempPositions.push_back((short) (trkX[k] * lengthScale[generatorIndex])); 
			//std::cout << j << " " << trkX[0] << " " << trkX[1] << " " << trkX[2] << std::endl;

			short* tempShort = (short*)(&j);//convert unsigned to signed in memory
			tempPositions.push_back(*tempShort);
			tempPositions.push_back((short)abs(t.PID()));

			//keep particles at their original position/momentum if they haven't been produced yet (from a delayed production vertex)
			if ((t.dt() - minT) * SQRT2*bunchLength / 100.0f / SPEED_OF_LIGHT_M_PER_NS >= j*timeStep) continue;

			//update positions for next iteration based on the momentum from this iteration
			glm::vec3 newX = glm::vec3();
			for (int k = 0; k < 3; k++) newX[k] = (float)(trkX[k] + trkP[k] / e * timeStep * SPEED_OF_LIGHT_M_PER_NS);

			//RK4 integration of ODE of momentum dp/dt = q(p x B)/E
			//unit conversion is needed b/c B is in Tesla (SI), t is in ns, but p,E,q are in natural units
			glm::vec3 k1 = glm::vec3();
			glm::vec3 pXB = glm::cross(trkP, B);
			for (int k = 0; k < 3; k++) k1[k] = (float)(t.Charge() / e * pXB[k] * timeStep * UNIT_CONVESTION_FOR_P);

			glm::vec3 k2 = glm::vec3();
			pXB = glm::cross(trkP + k1 * 0.5f, B);
			for (int k = 0; k < 3; k++) k2[k] = (float)(t.Charge() / e * pXB[k] * timeStep * UNIT_CONVESTION_FOR_P);

			glm::vec3 k3 = glm::vec3();
			pXB = glm::cross(trkP + k2 * 0.5f, B);
			for (int k = 0; k < 3; k++) k3[k] = (float)(t.Charge() / e * pXB[k] * timeStep * UNIT_CONVESTION_FOR_P);

			glm::vec3 k4 = glm::vec3();
			pXB = glm::cross(trkP + k3, B);
			for (int k = 0; k < 3; k++) k4[k] = (float)(t.Charge() / e * pXB[k] * timeStep * UNIT_CONVESTION_FOR_P);

			glm::vec3 newP = glm::vec3();
			for (int k = 0; k < 3; k++) newP[k] = (float)(trkP[k] + (k1[k] + 2 * k2[k] + 2 * k3[k] + k4[k]) / 6.0f);
			if ( (glm::length(newP) / e > 1.01f) && (isGoodPropagation == true)) {
				std::cout << "Numerical instability detected! Not outputting this track!" << std::endl;
				isGoodPropagation = false;
			}

			/*Simple euler integration (was giving timestep-dependent errors)
			//update positions for next iteration based on the momentum from this iteration
			glm::vec3 newX  = glm::vec3();
			for (int k = 0; k < 3; k++) newX[k] += (float)( trkX[k] + trkP[k] / e * timeStep * SPEED_OF_LIGHT_M_PER_NS);

			//update momenta for next iteration based on X, P, B (tricky part)
			glm::vec3 newP = glm::vec3();
			glm::vec3 pXB = glm::cross(trkP, B);
			for (int k = 0; k < 3; k++) newP[k] += (float)( trkP[k] + t.Charge() / e * pXB[k] * timeStep )
			*/

			trkX = newX;
			trkP = newP;
			//if (i == 0) std::cout <<"\n" << j<< " \n" << trkX[0] << " " << trkX[1] << " " << trkX[2] << std::endl;
			//if (i == 0) std::cout << trkP[0] << " " << trkP[1] << " " << trkP[2] << std::endl;
		}

		//if propagation was good, copy temp vector into master positions vector
		//the magic number 5 below is the number of shorts used to describe each vertex
		if (isGoodPropagation) {
			nTrack[generatorIndex]++;

			//with index buffer
			//unsigned int oldIndexArraySize = indexArray[generatorIndex].size();
			//trackOffsets[generatorIndex].push_back((oldIndexArraySize * sizeof(unsigned int)));

			//without index buffer
			unsigned int oldArraySize = vtxPositions[generatorIndex].size();
			trackOffsets[generatorIndex].push_back((GLsizei)(oldArraySize / 5));

			unsigned int points = 0;
			for (unsigned int i = 0; i < tempPositions.size(); i++) {
				vtxPositions[generatorIndex].push_back(tempPositions[i]);
				if (i % 5 == 0) {
					points++;
					//with index buffer
					//indexArray[generatorIndex].push_back(oldIndexArraySize + i / 5);//see above about the 5
				}
			}
			pointsOnTrack[generatorIndex].push_back((GLsizei)points);
		}
	}
	std::cout << "Simulation took: " << time.TimeSplit() << " s" << std::endl;
}

