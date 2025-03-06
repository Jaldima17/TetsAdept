#pragma once

#include "Objects.h"
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>

class FileWriter
{
public:
	FileWriter(const std::string& name);
	FileWriter();

	void write(const std::map < std::string, std::vector<Objects>>& objs);
	void write(std::map<std::string, std::set<Objects>>& objs);
private:
	std::ofstream _file;

};