#include "Field.h"
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <tuple>
#include "Object.h"
#include "Lens.h"





std::tuple<std::vector<Object>, std::vector<Object>> image_creation(Object orig, std::vector<Lens> l_list)
{
	std::vector<Lens>::iterator it = l_list.begin();

	Object last_image = orig, current_image;

	std::vector<Object> im_images;
	std::vector<Object> re_images;

	while (it != l_list.end())
	{
		Lens current_lens = *it;

		current_image = current_lens.distortion(last_image);

		if (current_image.height / last_image.height > 0)
		{
			im_images.push_back(current_image);
		}
		else if (current_image.height / last_image.height < 0)
		{
			re_images.push_back(current_image);
		}
		else
		{
			break;
		}

		last_image = current_image;
		++it;
	}

	std::tuple<std::vector<Object>, std::vector<Object>> ret = std::make_tuple(im_images, re_images);

	return ret;
}


Field::Field() :
	original(0, 1)
{
	std::vector<Lens> empty_l;
	std::vector<Object> empty_im;
	lenses = empty_l;
	im_image = empty_im;
	re_image = empty_im;
}



Field::Field(Object original, std::vector<Lens> lenses):
	original(original),
	lenses(lenses)
{
	std::sort(lenses.begin(), lenses.end());

	std::tuple<std::vector<Object>, std::vector<Object>> images = image_creation(original, lenses);

	im_image = std::get<0>(images);
	re_image = std::get<1>(images);
}


Field::Field(std::tuple<float, float> orignial_params, std::vector<std::tuple<lens_type, float, float>> lenses_params) :
	original(std::get<0>(orignial_params), std::get<1>(orignial_params))
{
	for (std::vector<std::tuple<lens_type, float, float>>::iterator it = lenses_params.begin(); it != lenses_params.end(); ++it)
	{
		Lens lens(std::get<0>(*it), std::get<1>(*it), std::get<2>(*it));
		lenses.push_back(lens);
	}


	std::sort(lenses.begin(), lenses.end());

	std::tuple<std::vector<Object>, std::vector<Object>> images = image_creation(original, lenses);

	im_image = std::get<0>(images);
	re_image = std::get<1>(images);
}


Field::Field(std::string File_text)
{
	std::stringstream test(File_text);
	std::string segment;
	std::vector<std::string> seg_list;

	while (std::getline(test, segment))
	{
		seg_list.push_back(segment);
	}

	seg_list.erase(seg_list.end() - 1);

	std::vector<std::string>::iterator it = seg_list.begin();

	std::string orig = *it++;
	if (it != seg_list.end())
	{
		++it;
	}

	std::vector<std::string> lens_list;
	std::vector<std::string> image_list;
	if (it != seg_list.end())
	{
		while (it != seg_list.end() && it->compare(""))
		{
			lens_list.push_back(*it++);
		}

		if (it != seg_list.end())
		{
			++it;

			while (it != seg_list.end())
			{
				image_list.push_back(*it++);
			}
		}
	}


	original = Object(orig);


	for (std::vector<std::string>::iterator iter = lens_list.begin(); iter != lens_list.end(); ++iter)
	{
		lenses.push_back(Lens(*iter));
	}

	if (!lenses.empty())
	{
		std::sort(lenses.begin(), lenses.end());
	}

	for (std::vector<std::string>::iterator iter = image_list.begin(); iter != image_list.end(); ++iter)
	{
		std::stringstream my_word(*iter);
		std::string part;
		std::vector<std::string> part_list;

		while (std::getline(my_word, part, ':'))
		{
			part_list.push_back(part);
		}

		std::vector<std::string>::iterator inner = part_list.begin();

		int image_type = std::stoi(*inner++);

		if (image_type == 0)
		{
			re_image.push_back(Object(*inner));
		}
		else
		{
			im_image.push_back(Object(*inner));
		}
	}
}


Field::Field(const Field& copy):
	original(copy.original),
	lenses(copy.lenses),
	im_image(copy.im_image),
	re_image(copy.re_image)
{
}


Field::~Field()
{
}


std::string Field::save_field()
{
	std::string field_info;
	std::string object_info = original.save_object();
	std::string lenses_info;
	std::string image_info;

	for (std::vector<Lens>::iterator it = lenses.begin(); it != lenses.end(); ++it)
	{
		std::string current_lens_info = it->save_lens();
		lenses_info.append("\n");
		lenses_info.append(current_lens_info);
	}

	for (std::vector<Object>::iterator it = re_image.begin(); it != re_image.end(); ++it)
	{
		std::string current_image_info = "0:";
		current_image_info.append(it->save_object());
		image_info.append("\n");
		image_info.append(current_image_info);
	}

	for (std::vector<Object>::iterator it = im_image.begin(); it != im_image.end(); ++it)
	{
		std::string current_image_info = "1:";
		current_image_info.append(it->save_object());
		image_info.append("\n");
		image_info.append(current_image_info);
	}

	field_info.append(object_info);
	field_info.append("\n");
	field_info.append(lenses_info);
	field_info.append("\n");
	field_info.append(image_info);

	return field_info;
}
