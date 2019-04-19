#include "Lens.h"
#include "Object.h"
#include <vector>
#include <string>
#include <sstream>


Lens::Lens(lens_type type, float x, float focus):
	type(type),
	x(x),
	focus(focus)
{
}

Lens::Lens(std::string lens_info)
{
	std::stringstream test(lens_info);
	std::string segment;
	std::vector<std::string> seg_list;

	while (std::getline(test, segment, ','))
	{
		seg_list.push_back(segment);
	}

	std::vector<std::string>::iterator it = seg_list.begin();

	std::string string_lens_type = *it++;
	std::string string_lens_x = *it++;
	std::string string_lens_focus = *it;

	type = static_cast<lens_type>(std::stoi(string_lens_type));
	x = std::stof(string_lens_x);
	focus = std::stof(string_lens_focus);
}


Lens::~Lens()
{
}


Object Lens::distortion(const Object& original)
{
	float image_x;
	float image_height;

	float object_distance = x - original.x;

	float one_over_x;

	if (type == Convex)
	{
		one_over_x = (1.0 / focus) - (1.0 / object_distance);
	}
	else
	{
		one_over_x = (-(1.0 / focus)) - (1.0 / object_distance);
	}

	if (one_over_x == 0)
		return Object::Null_Image;

	float image_distance = 1.0 / one_over_x;
	image_x = x + image_distance;

	float K = -(image_distance / object_distance);

	image_height = K * original.height;

	return Object(image_x, image_height);
}


bool Lens::operator<(Lens snd)
{
	return (x < snd.x);
}


std::string Lens::save_lens()
{
	std::string lens_info;
	std::string l_type;
	std::string lens_x = std::to_string(x);
	std::string lens_focus = std::to_string(focus);

	if (type == Convex)
	{
		l_type = std::to_string(0);
	}
	else
	{
		l_type = std::to_string(1);
	}

	lens_info.append(l_type);
	lens_info.append(",");
	lens_info.append(lens_x);
	lens_info.append(",");
	lens_info.append(lens_focus);

	return lens_info;
}

