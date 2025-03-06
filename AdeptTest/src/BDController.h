#pragma once

#include "Objects.h"
#include <firebird/Interface.h>
#include <vector>
#include <map>

class BDController
{
public:
	BDController(const std::string& name);
	~BDController();

	void createDB();
	void attachDB();
	void detachDB();

	bool insertDataFromFile(const std::vector<Objects>& objs);

	std::map<std::string, std::vector<Objects>> groupByDistanse();
	std::map<std::string, std::vector<Objects>> groupByName();

	std::map<std::string, std::vector<Objects>> groupByType();
	std::map<std::string, std::vector<Objects>> groupByTime();

private:
	static Firebird::IMaster* master;

	std::string _name;
	Firebird::IProvider* _prov;
	Firebird::IStatus* _st;
	Firebird::IUtil* _utl;
	Firebird::ThrowStatusWrapper _status;

	Firebird::IXpbBuilder* _dpb;

	Firebird::IAttachment* _att;
};