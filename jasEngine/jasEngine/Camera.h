#pragma once
#include "Object.h"

using namespace std;
using namespace glm;

class Camera
{
public:
	Camera();
	Camera(Transform &transform, RigidBody rigidBody);
	~Camera();

	Transform transform;
	RigidBody rigidBody;
	float fov;
	float aspect;
	float near;
	float far;
	mat4 camMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;

	void update();
	void applyForce(vec3 force);

private:
	void updateView();
	void updateProjection();

};

