#pragma once
#include "include/Track.h"
#include "include/VertexArray.h"
#include "include/VertexBuffer.h"
#include "include/VertexBufferLayout.h"
#include "include/IndexBuffer.h"
#include "include/Renderer.h"
#include "include/shader.h"
#include "glm/glm.hpp"
#include <cstdlib>
#include <cmath>
#include <vector>
#include <iostream>

class Event {
public:
	Event(double _startTime, double _animationTime, double _fadeTime, double _refreshTime);
	~Event();

	int Update(double _time);//return 1 if a new event was created
	void SetupDraw();
	void Draw(Renderer* r, Shader* s);

	float getAlphaModiferForFade();

	inline void setNTimeItervals(int n) { nTimeIntervals = n; }
	inline void setTimeStep(float t) { timeStep = t; }

	inline unsigned int getNTrack() const { return nTrack; }
	inline int GetNTimeIntervals() { return nTimeIntervals; }
	inline double GetStartTime() { return startTime; }
	inline double GetRefreshTime() { return refreshTime; }
	inline double GetAnimationTime() { return animationTime; }

	//rendering items
	VertexBufferLayout trackBufferLayout;
	IndexBuffer trkIB;
	std::vector< VertexArray* > trkVertexArrays;
	std::vector< VertexBuffer* > trkVertexBuffer;

private:
	double startTime;//time the event was made
	double refreshTime;//number of seconds to wait before making a new event
	double animationTime;//number of seconds animations should complete in
	double fadeStartTime;//number of seconds to wait until fading tracks (after animation and before refresh time)
	double timeSinceStart;//time elapsed since the event was made

	int nTimeIntervals;// how many time intervals to propagate the track
	float timeStep;   //in nanoseconds

	unsigned int nTrack;

	std::vector< Track > tracks;
	std::vector< std::vector< float > > vtxPositions;
	std::vector< unsigned int > indexArray;

	void MakeNewEvent();
	void DeleteVertexBuffers();
	void DeleteVertexArrays();

};