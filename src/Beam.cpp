#include "include/Beam.h"
#define FURTHEST_DIST_FROM_ORIGIN 50.0f

Beam::Beam(std::string collider, float _secondsToNSConversion)
	:secondToNSConversion(_secondsToNSConversion)
{
	mutex_pileup = new std::mutex();
	mutex_bunchLength = new std::mutex();

	beamlineBufferLayout.Push<float>(3);
	beamlineBufferLayout.Push<float>(1);

	for (unsigned int i = 0; i< 2 * nPointsAlongBeam; i++) beamlineIndices[i] = i;

	if (collider.find("LHC") != std::string::npos) SetToLHC();
	if (collider.find("LEP") != std::string::npos) SetToLEP();
}

Beam::Beam(bool _isFixedTarget, int _nPipes, float _bunchSpacing, float _bunchLength, float _secondsToNSConversion, beamType _beam1, float _energy1, beamType _beam2 = NOTHING, float _energy2 = 0)
	: isFixedTarget(_isFixedTarget), nPipes(_nPipes), bunchSpacing(_bunchSpacing), bunchLength(_bunchLength), 
	secondToNSConversion(_secondsToNSConversion), beam1(_beam1), energy1(_energy1), beam2(_beam2), energy2(_energy2)
{
	mutex_pileup = new std::mutex();

	beamlineBufferLayout.Push<float>(3);
	beamlineBufferLayout.Push<float>(1);

	for (unsigned int i = 0; i< 2 * nPointsAlongBeam; i++) beamlineIndices[i] = i;

	UpdateBeams();
}

Beam::~Beam() {
	delete mutex_pileup;
}

void Beam::SetPileup(float _pileup) {
	mutex_pileup->lock();
	pileup = _pileup;
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
		for (int i = 0; i < 4 * nPointsAlongBeam; i++) {
			if (i % 4 == 0) beamlineEndpoints[i] = FURTHEST_DIST_FROM_ORIGIN * i / (float) nPointsAlongBeam;//only set X coordinate
			else beamlineEndpoints[i] = 0.0f;
		}
	}
	else if (nPipes == 1) {
		for (int i = 0; i < 4* nPointsAlongBeam; i++) {
			if (i % 4 == 0) beamlineEndpoints[i] = -FURTHEST_DIST_FROM_ORIGIN + 2* FURTHEST_DIST_FROM_ORIGIN * i / 4 / (float)nPointsAlongBeam;//only set X coordinate
			else beamlineEndpoints[i] = 0.0f;
		}
	}
	else if (nPipes == 2) {
		for (int i = 0; i < 4* nPointsAlongBeam; i++) {
			if (i % 4 == 0) beamlineEndpoints[i] = -FURTHEST_DIST_FROM_ORIGIN + 2 * FURTHEST_DIST_FROM_ORIGIN * i / 4/ (float)nPointsAlongBeam;//set X coordinate
			else if (i % 4 == 2 ) beamlineEndpoints[i] = beamlineEndpoints[i-2]/100.0f;
			else beamlineEndpoints[i] = 0.0f;
		}
		for (int i = 4* nPointsAlongBeam; i < 8*nPointsAlongBeam; i++) {
			if (i % 4 == 0) beamlineEndpoints[i] = -FURTHEST_DIST_FROM_ORIGIN + 2 * FURTHEST_DIST_FROM_ORIGIN * (i- 4*nPointsAlongBeam) / 4 / (float)nPointsAlongBeam;//set X coordinate
			else if (i % 4 == 2) beamlineEndpoints[i] = -beamlineEndpoints[i - 2] / 100.0f;
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
	beamlineVertexBuffer = new VertexBuffer( beamlineEndpoints , 2 * nPointsAlongBeam * 4 * sizeof(float));
	beamlineVertexArray = new VertexArray();
	beamlineVertexArray->AddBuffer(*beamlineVertexBuffer, beamlineBufferLayout);
	isNewed = true;
}

void Beam::Draw(Renderer* r, Shader* s) {
	GLCall(glLineWidth(2));
	s->SetUniform4f("u_Color", 0.7f, 0.7f, 0.7f, 1.0f);
	s->SetUniform1f("u_fadeStartDist", 10.0f);
	s->SetUniform1f("u_fadeEndDist", FURTHEST_DIST_FROM_ORIGIN+1.0f);
	r->Draw(*beamlineVertexArray, beamlineIB, *s, GL_LINE_STRIP,nPointsAlongBeam);
	if( nPipes==2 ) r->Draw(*beamlineVertexArray, beamlineIB, *s, GL_LINE_STRIP, nPointsAlongBeam, nPointsAlongBeam);
}

void Beam::Start(float time) {
	startTime = time;
	isStarted = true;
}

void Beam::Stop() {
	isStarted = false;
}

void Beam::Update(float time) {

}