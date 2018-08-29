#pragma once
#include <cmath>

class Track {
public:
	Track(float _px, float _py, float _pz, int _charge, float _mass);
	~Track();

	inline int Charge() { return charge; }
	inline float P() { return sqrt(P2()); }
	inline float P2() { return px*px + py*py + pz*pz; }
	inline float Px() { return px; }
	inline float Py() { return py; }
	inline float Pz() { return pz; }
	inline float Mass() { return mass; }

private:
	float px;
	float py;
	float pz;
	int charge;
	float mass;
};