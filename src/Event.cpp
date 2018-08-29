#include "include/Event.h"
#define SPEED_OF_LIGHT_M_PER_NS 0.2998

float UniformRandom(float min, float max) {
	return min + (max - min) * (rand() % 10000) / 10000.0f;
}

Event::Event(double _startTime, double _animationTime, double _fadeStartTime, double _refreshTime)
	: startTime(_startTime), refreshTime(_refreshTime), fadeStartTime(_fadeStartTime), animationTime(_animationTime), timeSinceStart(0)
{
	nTimeIntervals = 60 * animationTime; //60 because we want 60 points per second
	timeStep = 10.0 / (float)nTimeIntervals; //10 seems to give decently length tracks, but can change it to get longer or shorter

	trackBufferLayout.Push<float>(3);
	trackBufferLayout.Push<float>(1);

	MakeNewEvent();
	SetupDraw();
}

Event::~Event() {
	DeleteVertexBuffers();
	DeleteVertexArrays();
}

int Event::Update(double _time) {
	timeSinceStart = _time - startTime;
	if (timeSinceStart > refreshTime) {
		MakeNewEvent();

		startTime = _time;
		timeSinceStart = 0;

		return 1;
	}

	return 0;
}

void Event::MakeNewEvent() {
	//free vectors memory
	std::vector<Track>().swap(tracks);
	std::vector< unsigned int >().swap(indexArray);
	std::vector< std::vector< float > >().swap(vtxPositions);

	//figure out how many particles we make and fill in random momenta
	nTrack = (unsigned int) (2 + rand() % 200);
	for (unsigned int i = 0; i < nTrack; i++) {
		//random momentum, with charge of -1 or +1
		Track t = Track(UniformRandom(-1, 1), UniformRandom(-1, 1), UniformRandom(-1, 1), -1 + 2 * (rand() % 2), 0.139f);
		std::vector< float > tempPositions;

		glm::vec3 trkX = glm::vec3();
		glm::vec3 trkP = glm::vec3(t.Px(),t.Py(),t.Pz());
		glm::vec3 B = glm::vec3(0.5f, 0.0f, 0.0f);

		//relativistic particle in B field is same as non-relativistic w/ mass term replaced by gamma*mass (energy)
		double e = sqrt(t.P2() + t.Mass()*t.Mass());

		for (unsigned int j = 0; j < (unsigned int) nTimeIntervals; j++) {
			if(i==0) indexArray.push_back(j);

			//write down positions
			for (int k = 0; k < 3; k++) tempPositions.push_back((float)trkX[k]);
			tempPositions.push_back( (float) j );//index position needed for fading the track in the track Shader

			//update positions for next iteration based on the momentum from this iteration
			glm::vec3 newX = glm::vec3();
			for (int k = 0; k < 3; k++) newX[k] += (float)(trkX[k] + trkP[k] / e * timeStep * SPEED_OF_LIGHT_M_PER_NS);

			//RK4 integration of ODE of momentum dp/dt = q(p x B)/E
			glm::vec3 k1 = glm::vec3();
			glm::vec3 pXB = glm::cross(trkP, B);
			for (int k = 0; k < 3; k++) k1[k] += (float)(t.Charge() / e * pXB[k] * timeStep);

			glm::vec3 k2 = glm::vec3();
			pXB = glm::cross(trkP + k1 * 0.5f, B);
			for (int k = 0; k < 3; k++) k2[k] += (float)(t.Charge() / e * pXB[k] * timeStep);

			glm::vec3 k3 = glm::vec3();
			pXB = glm::cross(trkP + k2 * 0.5f, B);
			for (int k = 0; k < 3; k++) k3[k] += (float)(t.Charge() / e * pXB[k] * timeStep);

			glm::vec3 k4 = glm::vec3();
			pXB = glm::cross(trkP + k3, B);
			for (int k = 0; k < 3; k++) k4[k] += (float)(t.Charge() / e * pXB[k] * timeStep);

			glm::vec3 newP = glm::vec3();
			for (int k = 0; k < 3; k++) newP[k] += (float)(trkP[k] + (k1[k] + 2 * k2[k] + 2 * k3[k] + k4[k]) / 6.0f);

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
		}
		tracks.push_back(t);
		vtxPositions.push_back(tempPositions);
	}
}


void Event::SetupDraw() {
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

void Event::Draw(Renderer* r, Shader* s) {
	unsigned int nIndicesToDraw = 1 + (unsigned int)nTimeIntervals * timeSinceStart / animationTime;
	if (nIndicesToDraw > nTimeIntervals) nIndicesToDraw = nTimeIntervals;

	for (unsigned int i = 0; i < nTrack; i++) {
		r->Draw(*(trkVertexArrays.at(i)), trkIB, *s, GL_LINE_STRIP, nIndicesToDraw);
	}
}

float Event::getAlphaModiferForFade() {
	if (timeSinceStart < fadeStartTime) return 1.0f;
	if (timeSinceStart > refreshTime) return 0.0f;
	return 1.0f - (timeSinceStart - fadeStartTime)/(refreshTime - fadeStartTime);
}

void Event::DeleteVertexBuffers() {
	unsigned int vecSize = trkVertexBuffer.size();
	for (unsigned int i = 0; i < vecSize; i++) delete trkVertexBuffer.at(i);
}

void Event::DeleteVertexArrays() {
	unsigned int vecSize = trkVertexArrays.size();
	for (unsigned int i = 0; i < vecSize; i++) delete trkVertexArrays.at(i);
}

