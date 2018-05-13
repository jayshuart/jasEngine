#include "stdafx.h"
#include "Object.h"

Object::Object()
{
}

Object::Object(Transform &transform, char * textureFile, RigidBody rigidBody, colliderType type)
{
	//pass in data
	this->transform = transform;
	textureFilename = textureFile;
	this->rigidBody = rigidBody;
	collider = type;

	//set world matrix
	transform.worldMatrix = translate(transform.location) * yawPitchRoll(transform.rotation.y, transform.rotation.x, transform.rotation.z) * scale(transform.size);
}

Object::~Object()
{
}

void Object::applyForce(vec3 force)
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

bool Object::collidesWith(Object &object)
{
	//check if either party is colliderless
	if (collider == colliderless || object.collider == colliderless)
	{
		//cant be colliding if one or both of them dont have a collider can they?
		return false;
	}

	//check sphere to sphere collision
	else if (collider == sphere && object.collider == sphere)
	{
		//get sum of radii
		float radii = transform.size.x + object.transform.size.x;

		//get vector distance between objects
		vec3 distance = 
		{
			object.transform.location.x - transform.location.x,
			object.transform.location.y - transform.location.y,
			object.transform.location.z - transform.location.z
		};

		//convert vector to magnitude float
		float length = ((distance.x * distance.x) + (distance.y * distance.y) + (distance.z * distance.z));
		float magnitude = sqrt(length);

		//check magnitude against radii
		if (magnitude >= radii)
		{
			//theyre not close! cant be touching
			return false;
		}
	}

	//check box to box collison
	else if (collider == box && object.collider == box)
	{
		//check x
		float xDist = transform.location.x - object.transform.location.x; //distance between x pos
		float xSizeSum = transform.size.x + object.transform.size.x; //sum of lengths
		if (xDist >= xSizeSum || (object.transform.location.x - (object.transform.size.x * 2)) >= transform.location.x)
		{
			//too far, cant be touching
			return false;
		}

		//check y
		float yDist = transform.location.y - object.transform.location.y; //distance between y pos
		float ySizeSum = transform.size.y + object.transform.size.y; //sum of lengths
		if (yDist >= ySizeSum || (object.transform.location.y - (object.transform.size.y * 2)) >= transform.location.y)
		{
			//too far, cant be touching
			return false;
		}

		//check z
		float zDist = transform.location.z - object.transform.location.z; //distance between z pos
		float zSizeSum = transform.size.z + object.transform.size.z; //sum of lengths
		if (zDist >= zSizeSum || (object.transform.location.z - (object.transform.size.z * 2)) >= transform.location.z)
		{
			//too far, cant be touching
			return false;
		}
	}

	//check box to sphere
	else if (collider == box && object.collider == sphere)
	{
		
	}

	//passed all checks, has to be colliding
	return true;
}
