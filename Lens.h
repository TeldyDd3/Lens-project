#pragma once
#include "Object.h"
#include <string>

enum lens_type {Convex, Concave};

class Lens
{
public:
	lens_type type;
	float x;
	float focus;

	Object distortion(const Object& previous_object);

	Lens(lens_type type, float x, float focus);
	Lens(std::string lens_info);
	~Lens();

	bool operator<(Lens);

	std::string save_lens();
};


