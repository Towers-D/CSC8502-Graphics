#pragma once
#include "Vector3.h"

class Plane {
public:
	Plane() {};
	Plane(const Vector3& normal, float distance, bool normalise = false);
	~Plane() {};

	void SetNormal(const Vector3& normal) { this->normal = normal; }
	Vector3 getNormal() const { return normal; }

	void SetDistance(float dist) { distance = dist; }
	float GetDistance() const { return distance; }

	bool SphereInPlane(const Vector3& position, float radius) const;

protected:
	Vector3 normal;
	float distance;
};

