#include "Object.h"
#include <string>
#include <vector>
#include <sstream>



Object::Object(float x, float height) :
	x(x),
	height(height)
{
}


Object::Object(const Object& copy)
{
	x = copy.x;
	height = copy.height;
}


Object::Object(std::string text)
{
	std::stringstream test(text);
	std::string segment;
	std::vector<std::string> seg_list;

	while (std::getline(test, segment, ','))
	{
		seg_list.push_back(segment);
	}

	std::vector<std::string>::iterator it = seg_list.begin();

	std::string string_object_x = *it++;
	std::string string_object_height = *it;

	x = std::stof(string_object_x);
	height = std::stof(string_object_height);
}

Object::Object():
	x(0),
	height(0)
{
}


Object::~Object()
{
}


Object Object::Null_Image(0, 0);


std::string Object::save_object()
{
	std::string object_info;
	std::string object_x = std::to_string(x);
	std::string object_height = std::to_string(height);

	object_info.append(object_x);
	object_info.append(",");
	object_info.append(object_height);

	return object_info;
}