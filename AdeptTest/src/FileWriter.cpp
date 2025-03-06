#include "FIleWriter.h"

FileWriter::FileWriter(const std::string& name)
{
	_file.open(name);
	if (!_file.is_open())
	{
		throw std::runtime_error("Failed to open file");
	}
	_file.clear();
}

FileWriter::FileWriter()
{
	if (_file.is_open())
	{
		_file.close();
	}
}

void FileWriter::write(const std::map<std::string, std::vector<Objects>>& objs)
{
	if (_file.is_open())
	{
		for (auto it = objs.begin(); it != objs.end(); ++it)
		{
			_file << (*it).first << ":" << std::endl;
			
			for (int i = 0; i < (*it).second.size(); i++)
			{
				Objects obj = (*it).second[i];
				_file << obj.name << " " << obj.x << " " << obj.y << " " << obj.type << " " << obj.time << std::endl;
			}
		}
	}
}

void FileWriter::write(std::map<std::string, std::set<Objects>>& objs)
{
	if (_file.is_open())
	{
		for (auto it = objs.begin(); it != objs.end(); ++it)
		{
			_file << (*it).first << ":" << std::endl;

			for (auto it2 = (*it).second.begin(); it2 != (*it).second.end(); it2++)
			{
				Objects obj = *it2;
				_file << obj.name << " " << obj.x << " " << obj.y << " " << obj.type << " " << obj.time << std::endl;
			}
		}
	}
}
