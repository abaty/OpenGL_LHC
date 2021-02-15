#include "include/Track.h"

Track::Track(float _px, float _py, float _pz, int _charge, float _mass, int _pid, float _dz, float _dt)
	:px(_px), py(_py), pz(_pz), charge(_charge), mass(_mass), pid(_pid), dZ(_dz), dT(_dt)
//Track::Track(float _px, float _py, float _pz, int _charge, float _mass)
//	:px(_px), py(_py), pz(_pz), charge(_charge), mass(_mass)
{

}

Track::~Track() {

}
