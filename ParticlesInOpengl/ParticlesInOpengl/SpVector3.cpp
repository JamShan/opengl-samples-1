
#include "SpVector3.h"

/*
	Three-Dimensional Vector For Single-Precision Floating-Point Format

	Naming Convention:

	pm - Parameter
	lv - Local Variable
*/

SpVector3::SpVector3() :
	x(0.0f), y(0.0f), z(0.0f) {}

SpVector3::SpVector3(SpVector3& vector) :
	x(vector.x), y(vector.y), z(vector.z) {}

SpVector3::SpVector3(float pmX, float pmY, float pmZ) :
	x(pmX), y(pmY), z(pmZ) {}

SpVector3::~SpVector3() {};
