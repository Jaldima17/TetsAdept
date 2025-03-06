#include "FileReader.h"

FileReader::FileReader(const std::string& name)
{
	_file.open(name);
	if (!_file.is_open()) {
		throw std::runtime_error("Failed to open file");
	}
}

FileReader::~FileReader()
{
	if (_file.is_open()) {
		_file.close();
	}
}

std::vector <Objects> FileReader::read()
{
	std::vector <Objects> objs;

	if (_file.is_open())
	{
		std::string type;
		double x;
		double y;
		std::string name;
		double time;

		while (_file >> name >> x >> y >> type >> time)
		{
			Objects obj;
			obj.name = name;
			obj.time = time;
			obj.type = type;
			obj.x = x;
			obj.y = y;

			objs.push_back(obj);
		}
	}
	return objs;
}
