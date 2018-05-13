#pragma once
#include <vector>
#include <string>
#include <map>
#include <math.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>


using namespace std;
using namespace glm;

enum colliderType 
{ 
	colliderless, //1
	box, //2
	sphere  //3
};

struct Transform
{
	vec3 location;
	vec3 rotation;
	vec3 size;
	mat4 worldMatrix;
};

struct RigidBody
{
	vec3 velocity;
	vec3 force;
	float mass;
};

class Object
{
public:
	Object();
	Object(Transform &transform, char* textureFile, RigidBody rigidBody, colliderType type);
	~Object();

	Transform transform;
	RigidBody rigidBody;
	colliderType collider;
	char* textureFilename;
	void applyForce(vec3 force);
	bool collidesWith(Object &object);
};

