#pragma once

#include <string>
#include <stdint.h>
#include <ctime>

struct Objects
{
	std::string type;
	double x;
	double y;
	std::string name;
	double time;

	bool operator<(const Objects& other) const {
		return name < other.name;
	}

	bool operator==(const Objects& other) const {
		return name == other.name;
	}
};