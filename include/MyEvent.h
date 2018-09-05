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
#include "include/Timer.h"
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <thread>
#include <time.h>


class MyEvent {
public:
	MyEvent(double _animationTime, double _fadeTime, Beam* _beam, MCGenerator* _mcGen);
	~MyEvent();

	int Update();//return 1 if a new event was created
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

	inline unsigned int getNTrack() const { return nTrack[renderIndex]; }
	inline int GetNTimeIntervals() { return nTimeIntervals; }
	inline double GetStartTime() { return startTime; }
	inline double GetRefreshTime() { return refreshTime; }
	inline double GetAnimationTime() { return animationTime; }
	inline double GetTrackLengthModifier() { return trackLengthModifier; }
	inline bool GetIsSettingUp() { return isSettingUpNextEvent; }

	inline float GetPtCut() { return ptCut; }

	//rendering items
	VertexBufferLayout trackBufferLayout;
	IndexBuffer trkIB;
	VertexArray* trkVertexArray;
	VertexBuffer* trkVertexBuffer;

private:
	Beam *beamline;
	MCGenerator *thisMCGenerator;

	double startTime;//time the event was started rendering
	double delayTime;//number of seconds to wait before displaying a new event
	double refreshTime;//number of seconds to wait after any delay before making a new event
	double animationTime;//number of seconds animations should complete in
	double fadeStartTime;//number of seconds to wait until fading tracks (after animation and before refresh time)
	double timeSinceStart;//time elapsed since the beam crossing signal was gotten from beam class
	double timeSinceEventStart;//time elapsed since the EVENT started to render (timeSinceStart - delayTime)
	double trackLengthModifier;

	bool isSettingUpNextEvent;
	bool isIndexBufferReady;
	bool isVertexBufferReady;
	bool isVertexAttributeObjectReady;

	bool doPtCut = false;
	float ptCut = 999999;
	bool doEtaCut = false;
	float etaCutLow = -999;
	float etaCutHigh = 999;

	unsigned short nTimeIntervals;// how many time intervals to propagate the track
	float timeStep;   //in nanoseconds

	unsigned char renderIndex = 1;
	unsigned char generatorIndex = 0;

	//one is on screen and one is being cleaned up and having a new event inserted
	float lengthScale[2];
	unsigned int nTrack[2];
	std::vector< Track > tracks[2];
	std::vector< short > vtxPositions[2];
	std::vector< unsigned int > indexArray[2];
	std::vector< GLsizei > pointsOnTrack[2];
	std::vector< unsigned int> trackOffsets[2];

	void ResetSetupBooleans();
	void GetNewEvent();
	void DeleteVertexBuffers();
	void DeleteVertexArrays();

	void FillEventBuffer();

	std::vector< std::thread > eventGeneratorThreads;//vector to hold our created thread(s)

};