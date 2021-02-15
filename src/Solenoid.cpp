#include "include/Solenoid.h"

//
//digits 10 specifies how many digits precision
/*
double k = 0.56734587;
double elliptic = boost::math::ellint_1(k, boost::math::policies::policy< boost::math::policies::digits10< 4 > >());*/

Solenoid::Solenoid(double _L, double _I, unsigned int _nTurns ,double _R1, double _R2, glm::vec3 _axis, glm::vec3 _center):
	L(_L), I(_I), nTurns(_nTurns), R1(_R1), R2(_R2), center(_center)
{
	axis = glm::normalize(_axis);
	if (L <= 0) L = 0.01;
}

Solenoid::~Solenoid() {

}

glm::vec3 Solenoid::getBField(glm::vec3 r) {
	glm::vec3 B = glm::vec3(0);

	//vector from center of solenoid
	glm::vec3 R = r - center;

	//calculate the z and rho directions (cylindrical coordinates)
	double z = glm::dot(R, axis); 
	glm::vec3 rhoDirection = R - (float)z*axis;
	double rho = glm::length(rhoDirection) + 0.01;//add 1 mm to prevent dividing by 0

	//if we are close to the center of the solenoid, rhoDirection will be undefined, so choose axis (shoudln't affect result much)
	if (rho < 0.02) rhoDirection = axis;

	//approximate this solenoid as a solenoid having a sheet of current at rho = a = average(R1,R2)
	double a = (R1 + R2) / 2.0;

	//following the wikipedia page for a solenoid
	//cross referenced with this page:
    //https://ntrs.nasa.gov/archive/nasa/casi.ntrs.nasa.gov/19980227402.pdf
	double zetaPlus = z + L / 2;
	double zetaMinus = z - L / 2;

	double h2 = geth2(a, rho);

	double kPlus = pow( getk2(a, rho, zetaPlus) , 0.5);
	double kMinus = pow( getk2(a, rho, zetaMinus) , 0.5);

	double Brho = -(MU0 * nTurns * I / (2 * PI * L)) * pow((a / rho), 0.5) * (rhoKernel(kPlus) - rhoKernel(kMinus));
	double Bz = (MU0 * nTurns * I / (4 * PI * L)) / pow(a * rho, 0.5) *
		(zKernel(a, rho, h2, kPlus, zetaPlus) - zKernel(a, rho, h2, kMinus, zetaMinus));

	glm::vec3 out = (float)Bz * axis + (float)Brho * glm::normalize(rhoDirection);

	return out;
}

double Solenoid::geth2(double a, double rho) {
	return 4 * a*rho / ((a + rho)*(a + rho));
}

double Solenoid::getk2(double a, double rho, double zeta) {
	return 4 * a*rho / ((a + rho)*(a + rho) + zeta*zeta);
}

//note that k instead of k2 is used for elliptic b/c boost conventions
double Solenoid::rhoKernel(double k) {
	double elliptic1 = boost::math::ellint_1(k, boost::math::policies::policy< boost::math::policies::digits10< 4 > >());
	double elliptic2 = boost::math::ellint_2(k, boost::math::policies::policy< boost::math::policies::digits10< 4 > >());
	return ((k*k - 2) / k) * elliptic1 + (2 / k)*elliptic2;
}
//note that k instead of k2 is used for elliptic b/c boost conventions
double Solenoid::zKernel(double a, double rho, double h2, double k, double zeta) {
	double elliptic1 = boost::math::ellint_1(k, boost::math::policies::policy< boost::math::policies::digits10< 4 > >());
	double elliptic3 = boost::math::ellint_3(k, h2, boost::math::policies::policy< boost::math::policies::digits10< 4 > >());
	return zeta*k* (elliptic1 + (a - rho) / (a + rho) * elliptic3);
}
