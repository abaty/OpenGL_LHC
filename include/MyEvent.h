#pragma once
#include "include/Track.h"
#include "include/VertexArray.h"
#include "include/VertexBuffer.h"
#include "include/VertexBufferLayout.h"
#include "include/IndexBuffer.h"
#include "include/Renderer.h"
#include "include/shader.h"
#include "include/Beam.h"
#include "include/MCGenerator.h"
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <thread>
#include <time.h>

class MyEvent {
public:
	MyEvent(double _startTime, double _animationTime, double _fadeTime, Beam* _beam, MCGenerator* _mcGen);
	~MyEvent();

	int Update(double _time);//return 1 if a new event was created
	void SetupDraw();
	void Draw(Renderer* r, Shader* s);

	float getAlphaModiferForFade();

	inline void setNTimeItervals(int n) { nTimeIntervals = n; }
	inline void setTimeStep(float t) { timeStep = t; }
	inline void setTrackLengthModifier(double m) { trackLengthModifier = m; }

	inline void EnablePtCut() { doPtCut = true; }
	inline void EnablePtCut(float cut) { doPtCut = true; SetPtCut(cut); }
	inline void DisablePtCut() { doPtCut = false; }
	inline void SetPtCut(float cut) { ptCut = cut; }

	inline void EnableEtaCut() { doEtaCut = true; }
	inline void EnableEtaCut(float cut) { doEtaCut = true; SetEtaCutLow(-cut); SetEtaCutHigh(cut); }
	inline void EnableEtaCut(float cutLow, float cutHigh) { doEtaCut = true; SetEtaCutLow(cutLow); SetEtaCutHigh(cutHigh); }
	inline void DisableEtaCut() { doEtaCut = false; }
	inline void SetEtaCutLow(float cut) { etaCutLow = cut; }
	inline void SetEtaCutHigh(float cut) { etaCutHigh = cut; }

	inline unsigned int getNTrack() const { return nTrack; }
	inline int GetNTimeIntervals() { return nTimeIntervals; }
	inline double GetStartTime() { return startTime; }
	inline double GetRefreshTime() { return refreshTime; }
	inline double GetAnimationTime() { return animationTime; }
	inline double GetTrackLengthModifier() { return trackLengthModifier; }

	inline float GetPtCut() { return ptCut; }

	//rendering items
	VertexBufferLayout trackBufferLayout;
	IndexBuffer trkIB;
	std::vector< VertexArray* > trkVertexArrays;
	std::vector< VertexBuffer* > trkVertexBuffer;

private:
	Beam *beamline;
	MCGenerator *thisMCGenerator;

	double startTime;//time the event was made
	double refreshTime;//number of seconds to wait before making a new event
	double animationTime;//number of seconds animations should complete in
	double fadeStartTime;//number of seconds to wait until fading tracks (after animation and before refresh time)
	double timeSinceStart;//time elapsed since the event was made
	double trackLengthModifier;

	bool doPtCut = false;
	float ptCut = 999999;
	bool doEtaCut = false;
	float etaCutLow = -999;
	float etaCutHigh = 999;

	int nTimeIntervals;// how many time intervals to propagate the track
	float timeStep;   //in nanoseconds

	unsigned int nTrack;
	std::vector< Track > tracks;
	std::vector< std::vector< float > > vtxPositions;
	std::vector< unsigned int > indexArray;

	//event buffer items that the thread works on before they are swapped with the items above
	unsigned int b_nTrack;
	std::vector< Track > b_tracks;
	std::vector< std::vector< float > > b_vtxPositions;
	std::vector< unsigned int > b_indexArray;

	void GetNewEvent();
	void DeleteVertexBuffers();
	void DeleteVertexArrays();

	void FillEventBuffer();

	std::vector< std::thread > eventGeneratorThreads;//vector to hold our created thread(s)

};