#include "FileReader.h"
#include "FileWriter.h"
#include "Grouper.h"
#include "BDController.h"
#include <ibase.h>
#include <firebird/Interface.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <Windows.h>

int main()
{
	std::vector<Objects> objs;

	bool opened = false;
	while (!opened)
	{
		try
		{
			std::cout << "Enter file name: ";
			std::string nameFile;
			std::cin >> nameFile;
			FileReader reader(nameFile);
			objs = reader.read();
			opened = true;
		}
		catch (...)
		{
			std::cout << "Incorrect file, try again" << std::endl;
		}
	}

	std::cout << "Enter database name: ";
	std::string nameBD;
	std::cin >> nameBD;

	BDController bd(nameBD);

	try
	{
		bd.attachDB();
	}
	catch (...)
	{
		bd.createDB();
		bd.insertDataFromFile(objs);
	}
	{
		auto resultBd = bd.groupByTime();
		auto resultAlg = Grouper::groupByTime(objs);

		FileWriter writerBd("groupByTimeFromBd.txt");
		writerBd.write(resultBd);

		FileWriter writerAlg("groupByTimeFromAlg.txt");
		writerAlg.write(resultAlg);
	}
	{
		auto resultBd = bd.groupByDistanse();
		auto resultAlg = Grouper::groupByDistanse(objs);

		FileWriter writerBd("groupByDistanseBd.txt");
		writerBd.write(resultBd);

		FileWriter writerAlg("groupByDistanseAlg.txt");
		writerAlg.write(resultAlg);
	}
	{
		auto resultBd = bd.groupByName();
		auto resultAlg = Grouper::groupByName(objs);

		FileWriter writerBd("groupByNameBd.txt");
		writerBd.write(resultBd);

		FileWriter writerAlg("groupByNameAlg.txt");
		writerAlg.write(resultAlg);
	}
	{
		auto resultBd = bd.groupByType();
		auto resultAlg = Grouper::groupByType(objs, 2);

		FileWriter writerBd("groupByTypeBd.txt");
		writerBd.write(resultBd);

		FileWriter writerAlg("groupByTypeAlg.txt");
		writerAlg.write(resultAlg);
	}
	bd.detachDB();

	std::cout << std::endl << "The results can be seen in the files:" << std::endl;
	std::cout << "groupByTimeFromBd.txt" << " " << "groupByDistanseAlg.txt" << std::endl;
	std::cout << "groupByDistanseBd.txt" << " " << "groupByDistanseAlg.txt" << std::endl;
	std::cout << "groupByNameBd.txt" << " " << "groupByNameAlg.txt" << std::endl;
	std::cout << "groupByTypeBd.txt" << " " << "groupByTypeAlg.txt" << std::endl;

	system("pause");

	return 0;
}
