#pragma once
#include "Objects.h"
#include <vector>
#include <map>
#include <set>

class Grouper
{
public:
	static std::map<std::string, std::vector<Objects>> groupByDistanse(const std::vector<Objects>& objs);
	static std::map<std::string, std::set<Objects>> groupByName(const std::vector<Objects>& objs);
	static std::map<std::string, std::set<Objects>> groupByType(const std::vector<Objects>& objs, const int n);
	static std::map<std::string, std::vector<Objects>> groupByTime(const std::vector<Objects>& objs);
private:

};