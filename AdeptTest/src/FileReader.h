#pragma once

#include "Objects.h"
#include <string>
#include <fstream>
#include <vector>

class FileReader
{
public:
	FileReader(const std::string& name);
	~FileReader();
	std::vector < Objects> read();
private:
	std::ifstream _file;
};