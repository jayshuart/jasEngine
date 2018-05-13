#include "stdafx.h"
#include "Camera.h"


Camera::Camera()
{
}


Camera::Camera(Transform & transform, RigidBody rigidBody)
{
	//pass in data
	this->transform = transform;
	this->rigidBody = rigidBody;
}

Camera::~Camera()
{
}


//METHODS -------------------------------------------------------------------------------------------------------------
void Camera::applyForce(vec3 force)
{
	//find acceleration
	vec3 acceleration = force / rigidBody.mass;

	//limit velocity
	float length = ((rigidBody.velocity.x * rigidBody.velocity.x) + (rigidBody.velocity.y * rigidBody.velocity.y) + (rigidBody.velocity.z * rigidBody.velocity.z));
	float magnitude = sqrt(length);

	if (magnitude < .5 && magnitude > -.5) //check if we are under the alloted velocity
	{
		//update velocity
		rigidBody.velocity += acceleration;
	}

	//update position
	transform.location += rigidBody.velocity;
}

void Camera::update()
{
	//make view and projection matricies
	updateView();
	updateProjection();

	//calc camera matrix
	camMatrix = projectionMatrix * viewMatrix;
}

//HELPER METHODS ------------------------------------------------------------------------------------------------------
void Camera::updateView()
{
	//move or turn cam
	mat3 rotMatrix = (mat3)yawPitchRoll(transform.rotation.y, transform.rotation.x, transform.rotation.z);

	vec3 center = transform.location + rotMatrix * vec3(0, 0, -1);
	vec3 up = rotMatrix * vec3(0, 1, 0);

	//make view matrix
	viewMatrix = lookAt(transform.location, center, up);
}

void Camera::updateProjection()
{
	//zoom or change aspect ratio
	float zoom = 1.f;
	int width = 800;
	int height = 600;

	fov = 3.14159f * .4f / zoom;
	aspect = (float)width / (float)height;
	near = .01f;
	far = 1000.f;

	//make view matrix
	projectionMatrix = perspective(fov, aspect, near, far);
}
