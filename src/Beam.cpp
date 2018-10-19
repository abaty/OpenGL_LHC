#include "include/Beam.h"
#define FURTHEST_DIST_FROM_ORIGIN 50.0f
#define SPEED_OF_LIGHT_M_PER_NS 0.2998f

Beam::Beam(std::string collider, float _bunchCrossingDelay, float _secondsToNSConversion)
	:secondToNSConversion(_secondsToNSConversion), bunchCrossingDelay(_bunchCrossingDelay)
{
	mutex_BPTXFlag = new std::mutex();
	mutex_pileup = new std::mutex();
	mutex_bunchLength = new std::mutex();

	beamlineBufferLayout.Push<float>(3);

	for (unsigned int i = 0; i< 2 * nPointsAlongBeam; i++) beamlineIndices[i] = i;

	if (collider.find("LHC") != std::string::npos) SetToLHC();
	if (collider.find("LEP") != std::string::npos) SetToLEP();
}

Beam::Beam(bool _isFixedTarget, int _nPipes, float _bunchSpacing, float _bunchLength, float _bunchCrossingDelay,float _secondsToNSConversion, beamType _beam1, float _energy1, beamType _beam2 = NOTHING, float _energy2 = 0)
	: isFixedTarget(_isFixedTarget), nPipes(_nPipes), bunchSpacing(_bunchSpacing), bunchLength(_bunchLength), 
	secondToNSConversion(_secondsToNSConversion), beam1(_beam1), energy1(_energy1), beam2(_beam2), energy2(_energy2),
	bunchCrossingDelay(_bunchCrossingDelay)
{
	mutex_BPTXFlag = new std::mutex();
	mutex_pileup = new std::mutex();
	mutex_bunchLength = new std::mutex();

	beamlineBufferLayout.Push<float>(3);
	beamlineBufferLayout.Push<float>(1);

	for (unsigned int i = 0; i< 2 * nPointsAlongBeam; i++) beamlineIndices[i] = i;

	UpdateBeams();
}

Beam::~Beam() {
	delete mutex_pileup;
	delete mutex_bunchLength;
	delete mutex_BPTXFlag;
}

void Beam::SetPileup(float _pileup) {
	mutex_pileup->lock();
	pileup = _pileup;
	mutex_pileup->unlock();
}

void Beam::SetBPTXFlag(bool _b) { 
	mutex_BPTXFlag->lock();
	BPTXFlag = _b;
	mutex_BPTXFlag->unlock();
};

void Beam::SetIsPoissonPU(bool _b) { 
	mutex_pileup->lock();
	isPoissonPU = _b; 
	mutex_pileup->unlock();
}

void Beam::SetIgnore0PU(bool _b) {
	mutex_pileup->lock();
	ignore0PU = _b; 
	mutex_pileup->unlock();
}

void Beam::SetToLHC(float _bunchSpacing, float _bunchLength, float CoMEnergy) {
	isFixedTarget = false;
	nPipes = 2;
	nBeams = 2;
	bunchSpacing = _bunchSpacing;
	bunchLength = _bunchLength;
	beam1 = beamType::P;
	beam2 = beamType::P;
	energy1 = CoMEnergy/2.0f;
	energy2 = CoMEnergy/2.0f;
	UpdateBeams();
}

void Beam::SetToLEP(float _bunchSpacing, float _bunchLength, float CoMEnergy) {
	isFixedTarget = false;
	nPipes = 2;
	nBeams = 2;
	bunchSpacing = _bunchSpacing;
	bunchLength = _bunchLength;
	beam1 = beamType::EPLUS;
	beam2 = beamType::EMINUS;
	energy1 = CoMEnergy / 2.0f;
	energy2 = CoMEnergy / 2.0f;
	UpdateBeams();
}

void Beam::UpdateBeams() {
	if (beam1 != beamType::NOTHING && beam2 != beamType::NOTHING) nBeams = 2;
	else if (beam1 != beamType::NOTHING || beam2 != beamType::NOTHING) {
		nBeams = 1;
		nPipes = 1;
	}
	else {
		std::cout << "Please specify a beam type!" << std::endl;
		nBeams = 0;
	}

	if (isFixedTarget) {
		for (int i = 0; i < 3 * nPointsAlongBeam; i++) {
			if (i % 3 == 0) beamlineEndpoints[i] = FURTHEST_DIST_FROM_ORIGIN * i / (float) nPointsAlongBeam;//only set X coordinate
			else beamlineEndpoints[i] = 0.0f;
		}
	}
	else if (nPipes == 1) {
		for (int i = 0; i < 3* nPointsAlongBeam; i++) {
			if (i % 3 == 0) beamlineEndpoints[i] = -FURTHEST_DIST_FROM_ORIGIN + 2* FURTHEST_DIST_FROM_ORIGIN * i / 3 / (float)nPointsAlongBeam;//only set X coordinate
			else beamlineEndpoints[i] = 0.0f;
		}
	}
	else if (nPipes == 2) {
		for (int i = 0; i < 3* nPointsAlongBeam; i++) {
			if (i % 3 == 0) beamlineEndpoints[i] = -FURTHEST_DIST_FROM_ORIGIN + 2 * FURTHEST_DIST_FROM_ORIGIN * i / 3/ (float)nPointsAlongBeam;//set X coordinate
			else if (i % 3 == 2 ) beamlineEndpoints[i] = beamlineEndpoints[i-2]/100.0f;
			else beamlineEndpoints[i] = 0.0f;
		}
		for (int i = 3* nPointsAlongBeam; i < 6*nPointsAlongBeam; i++) {
			if (i % 3 == 0) beamlineEndpoints[i] = -FURTHEST_DIST_FROM_ORIGIN + 2 * FURTHEST_DIST_FROM_ORIGIN * (i- 3*nPointsAlongBeam) / 3 / (float)nPointsAlongBeam;//set X coordinate
			else if (i % 3 == 2) beamlineEndpoints[i] = -beamlineEndpoints[i - 2] / 100.0f;
			else beamlineEndpoints[i] = 0.0f;
		}
	}
	else {
		std::cout << "error in beamline, wrong number of pipes!" << std::endl;
	}
	SetupDraw();
}

void Beam::SetupDraw() {
	//bind index arrays
	beamlineIB.AddData(beamlineIndices, 2 * nPointsAlongBeam);

	//check to see if we need to cleanup before doing more news
	if (isNewed) {
		delete beamlineVertexBuffer;
		delete beamlineVertexArray;
	}

	// bind buffers and VAOs
	beamlineVertexBuffer = new VertexBuffer( beamlineEndpoints , 2 * nPointsAlongBeam * 3 * sizeof(float));
	beamlineVertexArray = new VertexArray();
	beamlineVertexArray->AddBuffer(*beamlineVertexBuffer, beamlineBufferLayout);
	isNewed = true;
}

void Beam::Draw(Renderer* r, Shader* s) {
	if (!settings.doDrawBeamline) return;
	GLCall(glLineWidth(1));

	//is the beamlin is off, draw it regular (gray)
	float bunchCenter = (timeSinceLastBunchCrossing * secondToNSConversion - bunchCrossingDelay) * SPEED_OF_LIGHT_M_PER_NS;

	if (!isStarted) {
		s->SetUniform4f("u_BunchColor", 0.4f, 0.4f, 0.4f, 1.0f);
		s->SetUniform1f("u_BunchCenter", 0.0f);
	}
	else { // otherwise draw some bunches
		if		(beam1 == beamType::P		|| beam1 == beamType::PBAR)		s->SetUniform4f("u_BunchColor", 0.0f, 0.8f, 1.0f, 1.0f);
		else if (beam1 == beamType::EMINUS	|| beam1 == beamType::EPLUS)	s->SetUniform4f("u_BunchColor", 1.0f, 1.0f, 0.2f, 1.0f);
		else if (beam1 == beamType::MUMINUS	|| beam1 == beamType::MUPLUS)	s->SetUniform4f("u_BunchColor", 0.8f, 0.05f, 0.05f, 1.0f);
		else if (beam1 == beamType::PIMINUS	|| beam1 == beamType::PIPLUS)	s->SetUniform4f("u_BunchColor", 0.1f, 0.8f, 0.1f, 1.0f);
		else s->SetUniform4f("u_BunchColor", 1.0f, 0.1f, 0.6f, 1.0f);
		
		s->SetUniform1f("u_BunchCenter", -bunchCenter);
	}
	s->SetUniform4f("u_PipeColor", 0.2f, 0.2f, 0.2f, 1.0f);

	s->SetUniform1f("u_BunchSpacing", bunchSpacing * SPEED_OF_LIGHT_M_PER_NS);
	s->SetUniform1f("u_2BunchLengthSquared", (float)2*(bunchLength/100.0f)*(bunchLength/100.0f));//denominator of a gaussian

	s->SetUniform1f("u_fadeStartDist", 20.0f);
	s->SetUniform1f("u_fadeEndDist", FURTHEST_DIST_FROM_ORIGIN+1.0f);

	s->SetUniform1ui("u_bothWays", 0);
	if (!isFixedTarget && nPipes == 1) s->SetUniform1ui("u_bothWays", 1);

	r->Draw(*beamlineVertexArray, beamlineIB, *s, GL_LINE_STRIP,nPointsAlongBeam);

	//second pipe
	if (nPipes == 2) {
		if (!isStarted) {
			s->SetUniform4f("u_BunchColor", 0.4f, 0.4f, 0.4f, 1.0f);
			s->SetUniform1f("u_BunchCenter", 0.0f);
		}
		else { // otherwise draw some bunches
			if (beam2 == beamType::P			|| beam2 == beamType::PBAR)		s->SetUniform4f("u_BunchColor", 0.0f, 0.8f, 1.0f, 1.0f);
			else if (beam2 == beamType::EMINUS	|| beam2 == beamType::EPLUS)	s->SetUniform4f("u_BunchColor", 1.0f, 1.0f, 0.2f, 1.0f);
			else if (beam2 == beamType::MUMINUS || beam2 == beamType::MUPLUS)	s->SetUniform4f("u_BunchColor", 0.8f, 0.05f, 0.05f, 1.0f);
			else if (beam2 == beamType::PIMINUS || beam2 == beamType::PIPLUS)	s->SetUniform4f("u_BunchColor", 0.1f, 0.8f, 0.1f, 1.0f);
			else s->SetUniform4f("u_BunchColor", 1.0f, 0.1f, 0.6f, 1.0f);

			s->SetUniform1f("u_BunchCenter", bunchCenter);
		}
		r->Draw(*beamlineVertexArray, beamlineIB, *s, GL_LINE_STRIP, nPointsAlongBeam, nPointsAlongBeam);
	}
}

void Beam::Start() {
	startTime = (float)glfwGetTime();
	SetBPTXFlag(true);
	isStarted = true;
}

void Beam::Stop() {
	isStarted = false;
	SetBPTXFlag(false);
}

void Beam::UpdateTiming() {
	if (!isStarted) return;

	timeSinceLastBunchCrossing = (float)glfwGetTime() - startTime;
	if (timeSinceLastBunchCrossing > bunchSpacing / secondToNSConversion) {
		if (!GetBPTXFlag()) {
			Start();
		}
	}
}