#pragma once
#include <vector>
#include <string>
#include <tuple>
#include "Object.h"
#include "Lens.h"

class Field
{
public:
	Object original;
	std::vector<Lens> lenses;
	std::vector<Object> im_image;
	std::vector<Object> re_image;

	Field();
	Field(Object original_object, std::vector<Lens> lenses);
	Field(std::tuple<float, float> original_params, std::vector<std::tuple<lens_type, float, float>> lenses_params);
	Field(std::string field_description);
	Field(const Field& copy_field);
	~Field();

	std::string save_field();
};


std::tuple<std::vector<Object>, std::vector<Object>> image_creation(Object, std::vector<Lens>);

