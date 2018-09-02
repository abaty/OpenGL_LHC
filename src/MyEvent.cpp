#include "include/MyEvent.h"
#define SPEED_OF_LIGHT_M_PER_NS 0.2998
#define UNIT_CONVESTION_FOR_P 1.602*SPEED_OF_LIGHT_M_PER_NS/5.39
#define SQRT2 1.4142135

MyEvent::MyEvent(double _startTime, double _animationTime, double _fadeStartTime, Beam* _beam, MCGenerator* _mcGen)
	: startTime(_startTime), timeSinceStart(0)
{
	thisMCGenerator = _mcGen;
	beamline = _beam;
	trackLengthModifier = beamline->GetSecondsToNSConversion();
	refreshTime = beamline->GetBunchSpacing()/beamline->GetSecondsToNSConversion();
	animationTime = _animationTime * refreshTime;
	fadeStartTime = _fadeStartTime * refreshTime;

	nTimeIntervals = (int)( 60.0 * animationTime ); //60 because we want 60 points per second
	timeStep = (float) trackLengthModifier / 60.f; //each timestep is 1/60th of a ns (1s -> 1 ns) if modifier is 1
	
	trackBufferLayout.Push<float>(3);
	trackBufferLayout.Push<float>(1);

	eventGeneratorThreads.push_back(std::thread(&MyEvent::FillEventBuffer, this));//fill event buffer
	GetNewEvent();
	SetupDraw();
}

MyEvent::~MyEvent() {
	DeleteVertexBuffers();
	DeleteVertexArrays();
}

int MyEvent::Update(double _time) {
	timeSinceStart = _time - startTime;
	if (timeSinceStart > refreshTime) {
		GetNewEvent();

		startTime = glfwGetTime();
		timeSinceStart = 0;

		return 1;
	}

	return 0;
}


void MyEvent::SetupDraw() {
	//bind index arrays
	trkIB.AddData(indexArray.data(), nTimeIntervals);

	//clear old VertexArrays and Vertex Buffers
	DeleteVertexBuffers();
	DeleteVertexArrays();
	std::vector< VertexBuffer* >().swap(trkVertexBuffer);
	std::vector< VertexArray* >().swap(trkVertexArrays);

	for (unsigned int i = 0; i < nTrack; i++) {
		VertexBuffer* tempVB = new VertexBuffer((vtxPositions.at(i)).data(), nTimeIntervals * 4 * sizeof(float));
		trkVertexBuffer.push_back(tempVB);

		VertexArray* tempVA = new VertexArray();
		tempVA->AddBuffer(*(trkVertexBuffer.at(i)), trackBufferLayout);
		trkVertexArrays.push_back(tempVA);
	}
}

void MyEvent::Draw(Renderer* r, Shader* s) {
	unsigned int nIndicesToDraw = 1 + (unsigned int)(nTimeIntervals * timeSinceStart / animationTime);
	if (nIndicesToDraw > (unsigned int)nTimeIntervals) nIndicesToDraw = nTimeIntervals;

	GLCall(glLineWidth(2));
	s->SetUniform1f("u_fractionOfPropagationTimeElapsed", (float)(timeSinceStart) / (float)(animationTime));
	s->SetUniform1f("u_minAlpha", 0.5f);
	s->SetUniform1f("u_pointsOnTrack", (float)nTimeIntervals);
	s->SetUniform1f("u_nPointsOfGradient", 0.025f * nTimeIntervals);
	s->SetUniform1f("u_alphaModifierForFade", getAlphaModiferForFade());

	for (unsigned int i = 0; i < nTrack; i++) {
		if ( abs(tracks.at(i).PID()) == 13 ) {//muon
			s->SetUniform4f("u_Color", 0.8f, 0.05f, 0.05f, 0.0f);
			r->Draw(*(trkVertexArrays.at(i)), trkIB, *s, GL_LINE_STRIP, nIndicesToDraw);
		 } else if (abs(tracks.at(i).PID()) == 11) {//electron
			s->SetUniform4f("u_Color", 1.0f, 1.0f, 0.2f, 0.0f);
			r->Draw(*(trkVertexArrays.at(i)), trkIB, *s, GL_LINE_STRIP, nIndicesToDraw);
		 } else if (abs(tracks.at(i).PID()) == 2212) {//proton
			 s->SetUniform4f("u_Color", 0.0f, 0.8f, 1.0f, 0.0f);
			 r->Draw(*(trkVertexArrays.at(i)), trkIB, *s, GL_LINE_STRIP, nIndicesToDraw);
		 } else if (abs(tracks.at(i).PID()) == 321) {//kaon
			 s->SetUniform4f("u_Color", 0.6f, 0.2f, 1.0f, 0.0f);
			 r->Draw(*(trkVertexArrays.at(i)), trkIB, *s, GL_LINE_STRIP, nIndicesToDraw);
		} else if (abs(tracks.at(i).PID()) == 211) { //pion
			s->SetUniform4f("u_Color", 0.1f, 0.8f, 0.1f, 0.0f);
			r->Draw(*(trkVertexArrays.at(i)), trkIB, *s, GL_LINE_STRIP, nIndicesToDraw);
		} else { //other
			s->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 0.0f);
			r->Draw(*(trkVertexArrays.at(i)), trkIB, *s, GL_LINE_STRIP, nIndicesToDraw);
		}

	}
}

float MyEvent::getAlphaModiferForFade() {
	if (timeSinceStart < fadeStartTime) return 1.0f;
	if (timeSinceStart > refreshTime) return 0.0f;
	return (float) (1.0f - (timeSinceStart - fadeStartTime)/(refreshTime - fadeStartTime));
}

void MyEvent::DeleteVertexBuffers() {
	unsigned int vecSize = trkVertexBuffer.size();
	for (unsigned int i = 0; i < vecSize; i++) delete trkVertexBuffer.at(i);
}

void MyEvent::DeleteVertexArrays() {
	unsigned int vecSize = trkVertexArrays.size();
	for (unsigned int i = 0; i < vecSize; i++) delete trkVertexArrays.at(i);
}


void MyEvent::GetNewEvent() {
	(eventGeneratorThreads.at(0)).join();//wait for the event buffer thread to finish before swapping buffer with existing data

	//swap out buffer items for 'active' items being drawn
	nTrack = b_nTrack;
	tracks.swap(b_tracks);
	indexArray.swap(b_indexArray);
	vtxPositions.swap(b_vtxPositions);

	timeStep = (float)trackLengthModifier / 60.f; //each timestep is 1/60th of a ns (1s -> 1 ns)

	//empty thread container and launch a new thread to fill up the (now empty) event buffer
	std::vector< std::thread >().swap(eventGeneratorThreads);
	eventGeneratorThreads.push_back(std::thread(&MyEvent::FillEventBuffer, this));//fill event buffer
}

void MyEvent::FillEventBuffer() {
	//free vectors memory
	std::vector<Track>().swap(b_tracks);
	std::vector< unsigned int >().swap(b_indexArray);
	std::vector< std::vector< float > >().swap(b_vtxPositions);

	//get a SIGNAL event from our event generator
	thisMCGenerator->NewEvent(b_tracks);
	//add pileups if wanted
	beamline->mutex_pileup->lock();
	unsigned int extraInteractions = (unsigned int)beamline->GetPileup()+0.001-1;//add slight offset to take care of any weird float conversion errors
	beamline->mutex_pileup->unlock();
	for (unsigned int i = 0; i < extraInteractions; i++) {
		thisMCGenerator->NewEvent(b_tracks);
	}

	//get bunchlength
	beamline->mutex_bunchLength->lock();
	float bunchLength = beamline->GetBunchLength();
	beamline->mutex_bunchLength->unlock();

	//calculate time offset (for shifting events so that the 'first' event is produced at t=0 in the bunch crossing
	float minT = 9999.0;
	for (unsigned int i = 0; i < b_tracks.size(); i++) {
		if (b_tracks.at(i).dt() < minT) minT = b_tracks.at(i).dt();
	}

	//run simulation on event
	b_nTrack = b_tracks.size();
	for (unsigned int i = 0; i < b_tracks.size(); i++) {
		Track t = b_tracks.at(i);

		//apply relevant cuts
		if (doPtCut && t.Pt() < ptCut) {
			b_nTrack--;
			continue;
		}
		if (doEtaCut && (t.Eta() < etaCutLow || t.Eta() > etaCutHigh)) {
			b_nTrack--;
			continue;
		}

		std::vector< float > tempPositions;

		//setup initial parameters
		glm::vec3 trkX = glm::vec3( t.dz()*(SQRT2*bunchLength/100.0f) , 0.0f, 0.0f);//offset in z direction some
		glm::vec3 trkP = glm::vec3(t.Px(), t.Py(), t.Pz());
		glm::vec3 B = glm::vec3(3.8f, 0.0f, 0.0f);

		//relativistic particle in B field is same as non-relativistic w/ mass term replaced by gamma*mass (energy)
		double e = sqrt(t.P2() + t.Mass()*t.Mass());

		for (unsigned int j = 0; j < (unsigned int)nTimeIntervals; j++) {
			if (i == 0) b_indexArray.push_back(j);

			//write down positions
			for (int k = 0; k < 3; k++) tempPositions.push_back((float)trkX[k]);
			tempPositions.push_back((float)j);//index position needed for fading the track in the track Shader

			//keep particles at their original position/momentum if they haven't been produced yet (from a delayed production vertex)
			if ( (t.dt() - minT) * SQRT2*bunchLength/100.0f/SPEED_OF_LIGHT_M_PER_NS >= j*timeStep ) continue;
			
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
		b_vtxPositions.push_back(tempPositions);
	}
}

