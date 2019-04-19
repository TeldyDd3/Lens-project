#pragma once
#include <string>

class Object
{
public:
	float x;
	float height;

	Object(float x, float height);
	Object(const Object& copy);
	Object(std::string object_description);
	Object();
	~Object();
	
	static Object Null_Image;

	std::string save_object();
};


