#pragma once
#include "include/VertexArray.h"
#include "include/VertexBuffer.h"
#include "include/VertexBufferLayout.h"
#include "include/IndexBuffer.h"
#include <iostream>
#include <string>
#include <mutex>

enum beamType {
	NOTHING,
	P,
	PBAR,
	EMINUS,
	EPLUS,
	MUPLUS,
	MUMINUS,
	D,
	AU,
	XE,
	PB,
};

class Beam {
public:

	Beam(std::string collider, float _bunchCrossingDelay, float _secondsToNSConversion);//fast constructor that will implement presets with "SetTo*()" functions
	Beam(bool _isFixedTarget, int _nPipes, float _bunchSpacing, float _bunchLength, float _bunchCrossingDelay, float _secondsToNSConversion, beamType _beam1, float _energy1, beamType _beam2, float _energy2);
	~Beam();

	void SetupDraw();
	void Draw(Renderer* r, Shader* s);
	void Update(float time);
	void Start(float time);
	void Stop();

	inline float    GetBunchCrossingDelay() { return bunchCrossingDelay; }
	inline float    GetBunchSpacing() { return bunchSpacing; }
	inline float    GetBunchLength() { return bunchLength; }
	inline float    GetPileup() { return pileup; }
	inline beamType GetBeam1() { return beam1; }
	inline beamType GetBeam2() { return beam2; }
	inline float    GetEnergy1() { return energy1; }
	inline float    GetEnergy2() { return energy2; }
	inline int      GetNPipes() { return nPipes; }
	inline int      GetNBeams() { return nBeams; }
	inline bool     GetIsFixedTarget() { return isFixedTarget; }
	inline float    GetStartTime() { return startTime; }
	inline float	GetSecondsToNSConversion() { return secondToNSConversion; }

	inline void SetBunchCrossingDelay(float _b) { bunchCrossingDelay = _b; }
	inline void SetBunchSpacing(float _bunchSpacing) { bunchSpacing = _bunchSpacing;  }
	inline void SetBunchLength(float _bunchLength) { bunchLength = _bunchLength; }
	inline void SetEnergy1(float e) { energy1 = e; }
	inline void SetEnergy2(float e) { energy2 = e; }
	void SetPileup(float _pileup);//mutexed

	inline void SetNPipes(int n) { nPipes = n; UpdateBeams(); }
	inline void SetIsFixedTarget(bool is) { isFixedTarget = is; UpdateBeams(); }
	inline void SetBeam1(beamType _beam1) { beam1 = _beam1; UpdateBeams(); }
	inline void SetBeam2(beamType _beam2) { beam2 = _beam2; UpdateBeams(); }

	inline void	SetSecondsToNSConversion(float sToNS) { secondToNSConversion = sToNS; }

	void SetToLHC(float _bunchSpacing = 25, float _bunchLength = 8, float CoMEnergy = 13000);

	//LEP bunch spacing is huge at 22us, but I use 22ns here.  Maybe this causes an issue later?
	void SetToLEP(float _bunchSpacing = 22, float _bunchLength = 1 , float CoMEnergy = 91.2);

	VertexBufferLayout beamlineBufferLayout;
	IndexBuffer   beamlineIB;
	VertexArray*  beamlineVertexArray;
	VertexBuffer* beamlineVertexBuffer;

	std::mutex * mutex_pileup;
	std::mutex * mutex_bunchLength;

private:

	float startTime;
	float secondToNSConversion;

	float bunchCrossingDelay;//ns, should be less than the bunch spacing
	float bunchSpacing; //(ns)
	float bunchLength; //cm
	float pileup = 1;

	int nPipes;
	int nBeams;

	beamType beam1;
	beamType beam2;
	float energy1;//in GeV
	float energy2;//in GeV

	bool isFixedTarget = false;
	bool isNewed = false;
	bool isStarted = false;

	//note could optimize by suppressing the Y index here if we really needed to
	static const unsigned int nPointsAlongBeam = 5000;
	float beamlineEndpoints[2 * 4 * nPointsAlongBeam];//first set of values are x,y,z,alpha points of beam 1; second set of values are x,y,z values of points of beam2
	unsigned int beamlineIndices[2* nPointsAlongBeam];

	void UpdateBeams();
};