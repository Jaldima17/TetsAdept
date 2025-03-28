#include "BDController.h"
#include <firebird/Message.h>
#include <firebird/IdlFbInterfaces.h>
#include <firebird/Interface.h>
#include <ib_util.h>
#include <iberror.h>

Firebird::IMaster* BDController::master = Firebird::fb_get_master_interface();

const char* BDController::_sqlGroupByDistanse = "select name, x, y, type, cast(datediff(second from timestamp '1970-01-01 00:00:00' to data) as float),\
			case when (x*x+y*y) <=10000 then \'<100\'\
			when (x*x+y*y) <=1000000 then \'<1000\'\
			when (x*x+y*y) <=100000000 then \'<10000\'\
			else \'Слишком далеко\' end as distanse from objects_table\
			order by distanse;";

const char* BDController::_sqlInsert = "create table objects_table (\
			id integer generated by default as identity primary key,\
			name varchar(30),\
			x float,\
			y float,\
			type varchar(30),\
			data timestamp);";

const char* BDController::_sqlGroupByName = "select o.name, o.x, o.y, o.type, cast(datediff(second from timestamp '1970-01-01 00:00:00' to data) as float),\
						case\
							when upper(substring(o.name from 1 for 1)) similar to '[A-Z]'\
							then upper(substring(o.name from 1 for 1))\
							else '#'\
						end as grouped_name\
					from objects_table o\
					order by o.name;";

const char* BDController::_sqlGroupByType = "with TypeCounts AS \
				( select type, count(*) as cnt\
				from objects_table\
				group by type)\
				select o.name, o.x, o.y, o.type, cast(datediff(second from timestamp '1970-01-01 00:00:00' to data) as float),\
					case\
						when tc.cnt > 1 then o.type\
						else 'Разное'\
					end as grouped_type\
				from objects_table o\
				join typecounts tc on o.type = tc.type\
				order by o.name;";

const char* BDController::_sqlGroupByTime = "select o.name, o.x, o.y, o.type, cast(datediff(second from timestamp '1970-01-01 00:00:00' to data) as float),\
				case\
					when cast(o.data as date) = current_date\
						then 'Сегодня'\
					when cast(o.data as date) = current_date + 1\
						then 'Завтра'\
					when extract(year from o.data) = extract(year from current_date)\
						and extract(month from o.data) = extract(month from current_date)\
						then 'В этом месяце'\
					when extract(year from o.data) = extract(year from current_date)\
						then 'В этом году'\
					else 'Ранее'\
				end as grouped_name\
				from objects_table o\
				order by o.data;";

BDController::BDController(const std::string& name):
	_name(name),
	_prov(master->getDispatcher()),
	_st(master->getStatus()),
	_utl(master->getUtilInterface()),
	_status(_st)
{
	_dpb = _utl->getXpbBuilder(&_status, Firebird::IXpbBuilder::DPB, NULL, 0);

	_dpb->insertInt(&_status, isc_dpb_page_size, 4 * 1024);
	_dpb->insertString(&_status, isc_dpb_user_name, "SYSDBA");
	_dpb->insertString(&_status, isc_dpb_password, "masterkey");
}

BDController::~BDController()
{
}

void BDController::createDB()
{
	_att = _prov->createDatabase(&_status, _name.c_str(),
		_dpb->getBufferLength(&_status), _dpb->getBuffer(&_status));
}

void BDController::attachDB()
{
	_att = _prov->attachDatabase(&_status,  _name.c_str(),
		_dpb->getBufferLength(&_status), _dpb->getBuffer(&_status));
}

void BDController::detachDB()
{
	try
	{
		_att->detach(&_status);
	}
	catch (const Firebird::FbException& error)
	{
		char buf[256];
		_utl->formatStatus(buf, sizeof(buf), error.getStatus());
		fprintf(stderr, "%s\n", buf);
	}
}

bool BDController::insertDataFromFile(const std::vector<Objects>& objs)
{
	try {
		Firebird::ITransaction* tra = _att->startTransaction(&_status, 0, NULL);

		_att->execute(&_status, tra, 0, _sqlInsert,
			SQL_DIALECT_V6, NULL, NULL, NULL, NULL);

		tra->commitRetaining(&_status);

		for (auto obj : objs)
		{
			std::string data = "insert into objects_table (name, x, y, type, data) values(";
			data += "\'" + obj.name + "\'" + ", ";
			data += std::to_string(obj.x) + ", ";
			data += std::to_string(obj.y) + ", ";
			data += "\'" + obj.type + "\'" + ", ";
			data += "dateadd("+std::to_string(obj.time)+" second to timestamp \'1970-01-01 00:00:00\')";
			data += ");";

			_att->execute(&_status, tra, 0, data.c_str(),
				SQL_DIALECT_V6, NULL, NULL, NULL, NULL);
			tra->commitRetaining(&_status);

		}
		tra->commit(&_status);
	}
	catch (const Firebird::FbException& error)
	{
		char buf[256];
		_utl->formatStatus(buf, sizeof(buf), error.getStatus());
		fprintf(stderr, "%s\n", buf);
	}
	
	return true;
}

std::map<std::string, std::vector<Objects>> BDController::groupByDistanse()
{
	return groupBy(Distanse);
}

std::map<std::string, std::vector<Objects>> BDController::groupByName()
{
	return groupBy(Name);
}

std::map<std::string, std::vector<Objects>> BDController::groupByType()
{
	return groupBy(Type);
}

std::map<std::string, std::vector<Objects>> BDController::groupByTime()
{
	return groupBy(Time);
}

std::map<std::string, std::vector<Objects>> BDController::groupBy(Group group)
{
	const char* sql;
	std::map<std::string, std::vector<Objects>> objs;
	switch (group)
	{
	case Distanse:
		sql = _sqlGroupByDistanse;
		break;
	case Time:
		sql = _sqlGroupByTime;
		break;
	case Type:
		sql = _sqlGroupByType;
		break;
	case Name:
		sql = _sqlGroupByName;
		break;
	default:
		return objs;
		break;
	}

	try {
		Firebird::ITransaction* tra = _att->startTransaction(&_status, 0, NULL);
		{
			FB_MESSAGE(Output, Firebird::ThrowStatusWrapper,
				(FB_VARCHAR(30), name)
				(FB_FLOAT, x)
				(FB_FLOAT, y)
				(FB_VARCHAR(30), type)
				(FB_FLOAT, data)
				(FB_VARCHAR(30), grouped_name)
			) output(&_status, master);



			Firebird::IResultSet* curs = _att->openCursor(&_status, tra, 0, sql,
				SQL_DIALECT_V6, NULL, NULL, output.getMetadata(), NULL, 0);

			while (curs->fetchNext(&_status, output.getData()) == Firebird::IStatus::RESULT_OK)
			{
				Objects obj;
				obj.name = output->name.str;
				obj.x = output->x;
				obj.y = output->y;
				obj.type = output->type.str;
				obj.time = output->data;

				objs[output->grouped_name.str].push_back(obj);
			}
		}

		tra->commitRetaining(&_status);
		tra->commit(&_status);
	}
	catch (const Firebird::FbException& error)
	{
		char buf[256];
		_utl->formatStatus(buf, sizeof(buf), error.getStatus());
		fprintf(stderr, "%s\n", buf);
	}

	return objs;
}
