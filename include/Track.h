#pragma once
#include <cmath>

class Track {
public:
	Track(float _px, float _py, float _pz, int _charge, float _mass, int _pid, float _dz, float _dt);
	~Track();

	inline int Charge() { return charge; }
	inline float P() { return sqrt(P2()); }
	inline float P2() { return px*px + py*py + pz*pz; }
	inline float Pt() { return sqrt(py*py + pz*pz); }
	inline float Pt2() { return sqrt(py*py + pz*pz); }
	inline float Px() { return px; }
	inline float Py() { return py; }
	inline float Pz() { return pz; }
	inline float Mass() { return mass; }
	inline int PID() { return pid; }
	inline float dz() { return dZ; }
	inline float dt() { return dT; }
	inline float Eta() { float r = px / P();  return (r<1.0f) ? atanh(r) : ((r > 0) - (r < 0)) * 99999; } //the two sets of ()'s are just the sign() function

private:
	float px;
	float py;
	float pz;
	int charge;
	float mass;
	int pid;
	float dZ;
	float dT;
};