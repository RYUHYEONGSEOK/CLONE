#include "stdafx.h"
#include "MathHelper.h"


const float CMathHelper::Infinity = FLT_MAX;
const float CMathHelper::PI = 3.1415926535f;

CMathHelper::CMathHelper()
{
}
CMathHelper::~CMathHelper()
{
}

float CMathHelper::AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	// Quadrant I or IV
	if (x >= 0.0f)
	{
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); // in [-pi/2, +pi/2]

		if (theta < 0.0f)
			theta += 2.0f*PI; // in [0, 2*pi).
	}

	// Quadrant II or III
	else
		theta = atanf(y / x) + PI; // in [0, 2*pi).

	return theta;
}

FLOAT CMathHelper::DistanceVector(XMFLOAT3 l, XMFLOAT3 r)
{
	FLOAT a = l.x - r.x;
	FLOAT b = l.y - r.y;
	FLOAT c = l.z - r.z;
	return sqrtf(a*a + b*b + c*c);
}

XMVECTOR CMathHelper::TargetVector2D(XMFLOAT3 target, XMFLOAT3 origin)
{
	XMFLOAT3 fTarget(target.x - origin.x, 0, target.z - origin.z);
	XMVECTOR vTarget = XMLoadFloat3(&fTarget);

	return XMVector3Normalize(vTarget);
}

XMVECTOR CMathHelper::RandUnitVec3()
{
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while (true)
	{
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(CMathHelper::RandF(-1.0f, 1.0f), CMathHelper::RandF(-1.0f, 1.0f), CMathHelper::RandF(-1.0f, 1.0f), 0.0f);

		// Ignore points outside the unit sphere in order to get an even distribution 
		// over the unit sphere.  Otherwise points will clump more on the sphere near 
		// the corners of the cube.

		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		return XMVector3Normalize(v);
	}
}

XMVECTOR CMathHelper::RandHemisphereUnitVec3(XMVECTOR n)
{
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while (true)
	{
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(CMathHelper::RandF(-1.0f, 1.0f), CMathHelper::RandF(-1.0f, 1.0f), CMathHelper::RandF(-1.0f, 1.0f), 0.0f);

		// Ignore points outside the unit sphere in order to get an even distribution 
		// over the unit sphere.  Otherwise points will clump more on the sphere near 
		// the corners of the cube.

		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		// Ignore points in the bottom hemisphere.
		if (XMVector3Less(XMVector3Dot(n, v), Zero))
			continue;

		return XMVector3Normalize(v);
	}
}