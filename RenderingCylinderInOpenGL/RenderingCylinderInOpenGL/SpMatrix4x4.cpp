#include "SpMatrix4x4.h"
#include "SpVector3.h"

SpMatrix4x4::SpMatrix4x4() :

	m11(0.0f), m12(0.0f), m13(0.0f), m14(0.0f),
	m21(0.0f), m22(0.0f), m23(0.0f), m24(0.0f),
	m31(0.0f), m32(0.0f), m33(0.0f), m34(0.0f),
	m41(0.0f), m42(0.0f), m43(0.0f), m44(0.0f) {}

SpMatrix4x4::SpMatrix4x4(const SpMatrix4x4& matrix) :

	m11(matrix.m11), m12(matrix.m12), m13(matrix.m13), m14(matrix.m14),
	m21(matrix.m21), m22(matrix.m22), m23(matrix.m23), m24(matrix.m24),
	m31(matrix.m31), m32(matrix.m32), m33(matrix.m33), m34(matrix.m34),
	m41(matrix.m41), m42(matrix.m42), m43(matrix.m43), m44(matrix.m44) {}

SpMatrix4x4::SpMatrix4x4
	(float pm11, float pm12, float pm13, float pm14,
	float pm21, float pm22, float pm23, float pm24,
	float pm31, float pm32, float pm33, float pm34,
	float pm41, float pm42, float pm43, float pm44) :

	m11(pm11), m12(pm12), m13(pm13), m14(pm14),
	m21(pm21), m22(pm22), m23(pm23), m24(pm24),
	m31(pm31), m32(pm32), m33(pm33), m34(pm34),
	m41(pm41), m42(pm42), m43(pm43), m44(pm44) {}

SpMatrix4x4::~SpMatrix4x4() {}

void SpMatrix4x4::lookAtLh(const SpVector3& position, const SpVector3& target, const SpVector3& up)
{
	SpVector3 zAxis = (target - position);
	zAxis.normalize();

	SpVector3 xAxis = up.cross(zAxis);
	xAxis.normalize();

	SpVector3 yAxis = zAxis.cross(xAxis);

	m11 = xAxis.x;
	m21 = xAxis.y;
	m31 = xAxis.z;
	m41 = -xAxis.dot(position);

	m12 = yAxis.x;
	m22 = yAxis.y;
	m32 = yAxis.z;
	m42 = -yAxis.dot(position);

	m13 = zAxis.x;
	m23 = zAxis.y;
	m33 = zAxis.z;
	m43 = -zAxis.dot(position);

	m14 = 0.0f;
	m24 = 0.0f;
	m34 = 0.0f;
	m44 = 1.0f;
}

void SpMatrix4x4::invert()
{
	float lv11 =
		m22 * (m33 * m44 - m34 * m43) -
		m23 * (m32 * m44 - m34 * m42) +
		m24 * (m32 * m43 - m33 * m42);

	float lv21 =
		m21 * (m33 * m44 - m34 * m43) -
		m23 * (m31 * m44 - m34 * m41) +
		m24 * (m31 * m43 - m33 * m41);

	float lv31 =
		m21 * (m32 * m44 - m34 * m42) -
		m22 * (m31 * m44 - m34 * m41) +
		m24 * (m31 * m42 - m32 * m41);

	float lv41 =
		m21 * (m32 * m43 - m33 * m42) -
		m22 * (m31 * m43 - m33 * m41) +
		m23 * (m31 * m42 - m32 * m41);

	float lv12 =
		m12 * (m33 * m44 - m34 * m43) -
		m13 * (m32 * m44 - m34 * m42) +
		m14 * (m32 * m43 - m33 * m42);

	float lv22 =
		m11 * (m33 * m44 - m34 * m43) -
		m13 * (m31 * m44 - m34 * m41) +
		m14 * (m31 * m43 - m33 * m41);

	float lv32 =
		m11 * (m32 * m44 - m34 * m42) -
		m12 * (m31 * m44 - m34 * m41) +
		m14 * (m31 * m42 - m32 * m41);

	float lv42 =
		m11 * (m32 * m43 - m33 * m42) -
		m12 * (m31 * m43 - m33 * m41) +
		m13 * (m31 * m42 - m32 * m41);

	float lv13 =
		m12 * (m23 * m44 - m24 * m43) -
		m13 * (m22 * m44 - m24 * m42) +
		m14 * (m22 * m43 - m23 * m42);

	float lv23 =
		m11 * (m23 * m44 - m24 * m43) -
		m13 * (m21 * m44 - m24 * m41) +
		m14 * (m21 * m43 - m23 * m41);

	float lv33 =
		m11 * (m22 * m44 - m24 * m42) -
		m12 * (m21 * m44 - m24 * m41) +
		m14 * (m21 * m42 - m22 * m41);

	float lv43 =
		m11 * (m22 * m43 - m23 * m42) -
		m12 * (m21 * m43 - m23 * m41) +
		m13 * (m21 * m42 - m22 * m41);

	float lv14 =
		m12 * (m23 * m34 - m24 * m33) -
		m13 * (m22 * m34 - m24 * m32) +
		m14 * (m22 * m33 - m23 * m32);

	float lv24 =
		m11 * (m23 * m34 - m24 * m33) -
		m13 * (m21 * m34 - m24 * m31) +
		m14 * (m21 * m33 - m23 * m31);

	float lv34 =
		m11 * (m22 * m34 - m24 * m32) -
		m12 * (m21 * m34 - m24 * m31) +
		m14 * (m21 * m32 - m22 * m31);

	float lv44 =
		m11 * (m22 * m33 - m23 * m32) -
		m12 * (m21 * m33 - m23 * m31) +
		m13 * (m21 * m32 - m22 * m31);

	float lvDeterminant = m11 * lv11 - m12 * lv21 + m13 * lv31 - m14 * lv41;
	float lvOneDividedByDeterminant = 1.0f / lvDeterminant;

	lv12 = -lv12; lv14 = -lv14;
	lv21 = -lv21; lv23 = -lv23;
	lv32 = -lv32; lv34 = -lv34;
	lv41 = -lv41; lv43 = -lv43;

	lv11 *= lvOneDividedByDeterminant;
	lv12 *= lvOneDividedByDeterminant;
	lv13 *= lvOneDividedByDeterminant;
	lv14 *= lvOneDividedByDeterminant;

	lv21 *= lvOneDividedByDeterminant;
	lv22 *= lvOneDividedByDeterminant;
	lv23 *= lvOneDividedByDeterminant;
	lv24 *= lvOneDividedByDeterminant;

	lv31 *= lvOneDividedByDeterminant;
	lv32 *= lvOneDividedByDeterminant;
	lv33 *= lvOneDividedByDeterminant;
	lv34 *= lvOneDividedByDeterminant;

	lv41 *= lvOneDividedByDeterminant;
	lv42 *= lvOneDividedByDeterminant;
	lv43 *= lvOneDividedByDeterminant;
	lv44 *= lvOneDividedByDeterminant;

	m11 = lv11; m12 = lv12; m13 = lv13; m14 = lv14;
	m21 = lv21; m22 = lv22; m23 = lv23; m24 = lv24;
	m31 = lv31; m32 = lv32; m33 = lv33; m34 = lv34;
	m41 = lv41; m42 = lv42; m43 = lv43; m44 = lv44;
}

SpMatrix4x4 SpMatrix4x4::inverse() const
{
	float lv11 =
		m22 * (m33 * m44 - m34 * m43) -
		m23 * (m32 * m44 - m34 * m42) +
		m24 * (m32 * m43 - m33 * m42);

	float lv21 =
		m21 * (m33 * m44 - m34 * m43) -
		m23 * (m31 * m44 - m34 * m41) +
		m24 * (m31 * m43 - m33 * m41);

	float lv31 =
		m21 * (m32 * m44 - m34 * m42) -
		m22 * (m31 * m44 - m34 * m41) +
		m24 * (m31 * m42 - m32 * m41);

	float lv41 =
		m21 * (m32 * m43 - m33 * m42) -
		m22 * (m31 * m43 - m33 * m41) +
		m23 * (m31 * m42 - m32 * m41);

	float lv12 =
		m12 * (m33 * m44 - m34 * m43) -
		m13 * (m32 * m44 - m34 * m42) +
		m14 * (m32 * m43 - m33 * m42);

	float lv22 =
		m11 * (m33 * m44 - m34 * m43) -
		m13 * (m31 * m44 - m34 * m41) +
		m14 * (m31 * m43 - m33 * m41);

	float lv32 =
		m11 * (m32 * m44 - m34 * m42) -
		m12 * (m31 * m44 - m34 * m41) +
		m14 * (m31 * m42 - m32 * m41);

	float lv42 =
		m11 * (m32 * m43 - m33 * m42) -
		m12 * (m31 * m43 - m33 * m41) +
		m13 * (m31 * m42 - m32 * m41);

	float lv13 =
		m12 * (m23 * m44 - m24 * m43) -
		m13 * (m22 * m44 - m24 * m42) +
		m14 * (m22 * m43 - m23 * m42);

	float lv23 =
		m11 * (m23 * m44 - m24 * m43) -
		m13 * (m21 * m44 - m24 * m41) +
		m14 * (m21 * m43 - m23 * m41);

	float lv33 =
		m11 * (m22 * m44 - m24 * m42) -
		m12 * (m21 * m44 - m24 * m41) +
		m14 * (m21 * m42 - m22 * m41);

	float lv43 =
		m11 * (m22 * m43 - m23 * m42) -
		m12 * (m21 * m43 - m23 * m41) +
		m13 * (m21 * m42 - m22 * m41);

	float lv14 =
		m12 * (m23 * m34 - m24 * m33) -
		m13 * (m22 * m34 - m24 * m32) +
		m14 * (m22 * m33 - m23 * m32);

	float lv24 =
		m11 * (m23 * m34 - m24 * m33) -
		m13 * (m21 * m34 - m24 * m31) +
		m14 * (m21 * m33 - m23 * m31);

	float lv34 =
		m11 * (m22 * m34 - m24 * m32) -
		m12 * (m21 * m34 - m24 * m31) +
		m14 * (m21 * m32 - m22 * m31);

	float lv44 =
		m11 * (m22 * m33 - m23 * m32) -
		m12 * (m21 * m33 - m23 * m31) +
		m13 * (m21 * m32 - m22 * m31);

	float lvDeterminant = m11 * lv11 - m12 * lv21 + m13 * lv31 - m14 * lv41;
	float lvOneDividedByDeterminant = 1.0f / lvDeterminant;

	lv12 = -lv12; lv14 = -lv14;
	lv21 = -lv21; lv23 = -lv23;
	lv32 = -lv32; lv34 = -lv34;
	lv41 = -lv41; lv43 = -lv43;

	lv11 *= lvOneDividedByDeterminant;
	lv12 *= lvOneDividedByDeterminant;
	lv13 *= lvOneDividedByDeterminant;
	lv14 *= lvOneDividedByDeterminant;

	lv21 *= lvOneDividedByDeterminant;
	lv22 *= lvOneDividedByDeterminant;
	lv23 *= lvOneDividedByDeterminant;
	lv24 *= lvOneDividedByDeterminant;

	lv31 *= lvOneDividedByDeterminant;
	lv32 *= lvOneDividedByDeterminant;
	lv33 *= lvOneDividedByDeterminant;
	lv34 *= lvOneDividedByDeterminant;

	lv41 *= lvOneDividedByDeterminant;
	lv42 *= lvOneDividedByDeterminant;
	lv43 *= lvOneDividedByDeterminant;
	lv44 *= lvOneDividedByDeterminant;

	return SpMatrix4x4
		(lv11, lv12, lv13, lv14,
		lv21, lv22, lv23, lv24,
		lv31, lv32, lv33, lv34,
		lv41, lv42, lv43, lv44);
}

float SpMatrix4x4::determinant() const
{
	return
		m11 * (m22 * (m33 * m44 - m34 * m43) - m23 * (m32 * m44 - m34 * m42) + m24 * (m32 * m43 - m33 * m42)) -
		m12 * (m21 * (m33 * m44 - m34 * m43) - m23 * (m31 * m44 - m34 * m41) + m24 * (m31 * m43 - m33 * m41)) +
		m13 * (m21 * (m32 * m44 - m34 * m42) - m22 * (m31 * m44 - m34 * m41) + m24 * (m31 * m42 - m32 * m41)) -
		m14 * (m21 * (m32 * m43 - m33 * m42) - m22 * (m31 * m43 - m33 * m41) + m23 * (m31 * m42 - m32 * m41));
}

SpMatrix4x4 SpMatrix4x4::operator * (const SpMatrix4x4& matrix) const
{
	return SpMatrix4x4
		(m11 * matrix.m11 + m12 * matrix.m21 + m13 * matrix.m31 + m14 * matrix.m41,
		m11 * matrix.m12 + m12 * matrix.m22 + m13 * matrix.m32 + m14 * matrix.m42,
		m11 * matrix.m13 + m12 * matrix.m23 + m13 * matrix.m33 + m14 * matrix.m43,
		m11 * matrix.m14 + m12 * matrix.m24 + m13 * matrix.m34 + m14 * matrix.m44,

		m21 * matrix.m11 + m22 * matrix.m21 + m23 * matrix.m31 + m24 * matrix.m41,
		m21 * matrix.m12 + m22 * matrix.m22 + m23 * matrix.m32 + m24 * matrix.m42,
		m21 * matrix.m13 + m22 * matrix.m23 + m23 * matrix.m33 + m24 * matrix.m43,
		m21 * matrix.m14 + m22 * matrix.m24 + m23 * matrix.m34 + m24 * matrix.m44,

		m31 * matrix.m11 + m32 * matrix.m21 + m33 * matrix.m31 + m34 * matrix.m41,
		m31 * matrix.m12 + m32 * matrix.m22 + m33 * matrix.m32 + m34 * matrix.m42,
		m31 * matrix.m13 + m32 * matrix.m23 + m33 * matrix.m33 + m34 * matrix.m43,
		m31 * matrix.m14 + m32 * matrix.m24 + m33 * matrix.m34 + m34 * matrix.m44,

		m41 * matrix.m11 + m42 * matrix.m21 + m43 * matrix.m31 + m44 * matrix.m41,
		m41 * matrix.m12 + m42 * matrix.m22 + m43 * matrix.m32 + m44 * matrix.m42,
		m41 * matrix.m13 + m42 * matrix.m23 + m43 * matrix.m33 + m44 * matrix.m43,
		m41 * matrix.m14 + m42 * matrix.m24 + m43 * matrix.m34 + m44 * matrix.m44);
}

SpMatrix4x4& SpMatrix4x4::operator *= (const SpMatrix4x4& matrix)
{
	float lv11 = m11 * matrix.m11 + m12 * matrix.m21 + m13 * matrix.m31 + m14 * matrix.m41;
	float lv12 = m11 * matrix.m12 + m12 * matrix.m22 + m13 * matrix.m32 + m14 * matrix.m42;
	float lv13 = m11 * matrix.m13 + m12 * matrix.m23 + m13 * matrix.m33 + m14 * matrix.m43;
	float lv14 = m11 * matrix.m14 + m12 * matrix.m24 + m13 * matrix.m34 + m14 * matrix.m44;

	float lv21 = m21 * matrix.m11 + m22 * matrix.m21 + m23 * matrix.m31 + m24 * matrix.m41;
	float lv22 = m21 * matrix.m12 + m22 * matrix.m22 + m23 * matrix.m32 + m24 * matrix.m42;
	float lv23 = m21 * matrix.m13 + m22 * matrix.m23 + m23 * matrix.m33 + m24 * matrix.m43;
	float lv24 = m21 * matrix.m14 + m22 * matrix.m24 + m23 * matrix.m34 + m24 * matrix.m44;

	float lv31 = m31 * matrix.m11 + m32 * matrix.m21 + m33 * matrix.m31 + m34 * matrix.m41;
	float lv32 = m31 * matrix.m12 + m32 * matrix.m22 + m33 * matrix.m32 + m34 * matrix.m42;
	float lv33 = m31 * matrix.m13 + m32 * matrix.m23 + m33 * matrix.m33 + m34 * matrix.m43;
	float lv34 = m31 * matrix.m14 + m32 * matrix.m24 + m33 * matrix.m34 + m34 * matrix.m44;

	float lv41 = m41 * matrix.m11 + m42 * matrix.m21 + m43 * matrix.m31 + m44 * matrix.m41;
	float lv42 = m41 * matrix.m12 + m42 * matrix.m22 + m43 * matrix.m32 + m44 * matrix.m42;
	float lv43 = m41 * matrix.m13 + m42 * matrix.m23 + m43 * matrix.m33 + m44 * matrix.m43;
	float lv44 = m41 * matrix.m14 + m42 * matrix.m24 + m43 * matrix.m34 + m44 * matrix.m44;

	m11 = lv11; m12 = lv12; m13 = lv13; m14 = lv14;
	m21 = lv21; m22 = lv22; m23 = lv23; m24 = lv24;
	m31 = lv31; m32 = lv32; m33 = lv33; m34 = lv34;
	m41 = lv41; m42 = lv42; m43 = lv43; m44 = lv44;

	return *this;
}
