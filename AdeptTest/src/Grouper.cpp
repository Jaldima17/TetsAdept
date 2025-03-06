#include "Grouper.h"
#include <unordered_map>
#include <ctime>
#include <chrono>
#include <stdint.h>

struct Date
{
	int day;
	int month;
	int year;
};

Date unixTimeToHumanReadable(const uint32_t seconds)
{
    int daysOfMonth[] = { 31, 28, 31, 30, 31, 30,
                          31, 31, 30, 31, 30, 31 };

    long int currYear, daysTillNow, extraTime, extraDays,
        index, date, month, hours, minutes, secondss,
        flag = 0;

    daysTillNow = seconds / (24 * 60 * 60);
    extraTime = seconds % (24 * 60 * 60);
    currYear = 1970;

    while (true) {
        if (currYear % 400 == 0
            || (currYear % 4 == 0 && currYear % 100 != 0)) {
            if (daysTillNow < 366) {
                break;
            }
            daysTillNow -= 366;
        }
        else {
            if (daysTillNow < 365) {
                break;
            }
            daysTillNow -= 365;
        }
        currYear += 1;
    }

    extraDays = daysTillNow + 1;

    if (currYear % 400 == 0
        || (currYear % 4 == 0 && currYear % 100 != 0))
        flag = 1;

    month = 0, index = 0;
    if (flag == 1) {
        while (true) {

            if (index == 1) {
                if (extraDays - 29 < 0)
                    break;
                month += 1;
                extraDays -= 29;
            }
            else {
                if (extraDays - daysOfMonth[index] < 0) {
                    break;
                }
                month += 1;
                extraDays -= daysOfMonth[index];
            }
            index += 1;
        }
    }
    else {
        while (true) {

            if (extraDays - daysOfMonth[index] < 0) {
                break;
            }
            month += 1;
            extraDays -= daysOfMonth[index];
            index += 1;
        }
    }

    if (extraDays > 0) {
        month += 1;
        date = extraDays;
    }
    else {
        if (month == 2 && flag == 1)
            date = 29;
        else {
            date = daysOfMonth[month - 1];
        }
    }

    Date data;
    data.day = date;
    data.month = month;
    data.year = currYear;
    return data;
}
std::map<std::string, std::vector<Objects>> Grouper::groupByDistanse(const std::vector<Objects>& objs)
{
	std::map<std::string, std::vector<Objects>> result;

	for (auto o : objs)
	{
		double len = o.x * o.x + o.y * o.y;
		if (len < 10000)
			result["<100"].push_back(o);
		else if (len < 1000000)
			result["<1000"].push_back(o);
		else if (len < 100000000)
			result["<10000"].push_back(o);
		else result["Слишком далеко"].push_back(o);
	}
	return result;
}

std::map<std::string, std::set<Objects>> Grouper::groupByName(const std::vector<Objects>& objs)
{
	std::map<std::string, std::set<Objects>> result;

	for (auto o : objs)
	{
		if (!o.name.empty() && 'A' <= o.name[0] && o.name[0] <= 'Z')
			result[o.name.substr(0, 1)].insert(o);
		else result["#"].insert(o);
	}
	return result;
}

std::map<std::string, std::set<Objects>> Grouper::groupByType(const std::vector<Objects>& objs, const int n)
{
	std::map<std::string, std::set<Objects>> result;

	std::unordered_map<std::string, int> typeCount;

	for (auto o : objs)
	{
		typeCount[o.type]++;
	}

	for (auto o : objs)
	{
		if (typeCount[o.type] > n)
		{
			result[o.type].insert(o);
		}
		else
			result["Разное"].insert(o);
	}
	return result;
}

std::map<std::string, std::vector<Objects>> Grouper::groupByTime(const std::vector<Objects>& objs)
{
	std::map<std::string, std::vector<Objects>> result;

    for (auto o : objs)
    {
        Date currDate = unixTimeToHumanReadable(time(0));
        Date objDate = unixTimeToHumanReadable(o.time);

        if (objDate.day == currDate.day && objDate.month == currDate.month && objDate.year == currDate.year)
            result["Сегодня"].push_back(o);
        else if (objDate.day == currDate.day + 1 && objDate.month == currDate.month && objDate.year == currDate.year) 
            result["Сегодня"].push_back(o);
        else if (objDate.month == currDate.month && objDate.year == currDate.year)
            result["В этом месяце"].push_back(o);
        else if (objDate.year == currDate.year)
            result["В этом году"].push_back(o);
        else result["Ранее"].push_back(o);
    }
    return result;
}
