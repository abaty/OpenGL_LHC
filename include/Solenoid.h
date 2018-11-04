#pragma once
#include <boost/math/special_functions/ellint_1.hpp>
#include <boost/math/special_functions/ellint_2.hpp>
#include <boost/math/special_functions/ellint_3.hpp>
#include "glm/glm.hpp"
#ifndef PI
	#define PI 3.14592f
#endif
#ifndef MU0
	#define MU0 1.25664e-6
#endif

class Solenoid {
public:

	Solenoid(double _L, double _I, unsigned int _nTurns, double _R1, double _R2, glm::vec3 _axis, glm::vec3 _center);
	~Solenoid();

	double getL() { return L; };
	double getI() { return I; };
	double getR1() { return R1; };
	double getR2() { return R2; };
	unsigned int getNTurns() { return nTurns; };
	glm::vec3 getAxis() { return axis; };
	glm::vec3 getCenter() { return center; };

	void setL(double _L) { L = _L; };
	void setI(double _I) { I = _I; };
	void setR1(double _R1) { R1 = _R1; };
	void setR2(double _R2) { R2 = _R2; };
	void setNTurns(double n) { nTurns = n; };
	void setAxis(glm::vec3 _axis) { axis = _axis; };
	void setCenter(glm::vec3 _center) { center = _center; };

	glm::vec3 getBField(glm::vec3 r);

private:
	double L, R1, R2, I;
	unsigned int nTurns = 1;
	glm::vec3 axis;
	glm::vec3 center;

	double geth2(double a, double rho);
	double getk2(double a, double rho, double zeta);
	double rhoKernel(double k);
	double zKernel(double a, double rho, double h2, double k, double zeta);
};