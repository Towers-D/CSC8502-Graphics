#pragma once

#include "Vector4.h"
#include "Vector3.h"

class Light {
public:
	Light() {};
	Light(Vector3 position, Vector4 col, float radius) : position(position), spec(col), diff(col), radius(radius) {};
	Light(Vector3 position, Vector4 specCol, Vector4 diffCol, float radius) : position(position), spec(specCol), diff(diffCol), radius(radius) {};
	~Light() {};

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	Vector4 GetDiff() const { return diff; }
	void SetDiff(Vector4 val) { diff = val; }

	Vector4 GetSpec() const { return spec; }
	void SetSpec(Vector4 val) { spec = val; }

protected:
	Vector3 position;
	Vector4 diff;
	Vector4 spec;
	float radius;
};