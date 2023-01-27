//*****************************************************************************************
/// This program generates the Quandle Topologies for a specific order. It takes the
/// topology file name and quandle file name as the inputs and downloads the quandle topology
/// file to the local folder.
//*****************************************************************************************

#include "QuandleTopologies.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <sstream>
#include <codecvt>
#include <locale>

#define OUTFILE_PATH "C:\Tushar\\Temp\\"

using namespace std;

void StrReplace(std::string& str, const std::string& from, const std::string& to) {
	do {
		size_t start_pos = str.find(from);
		if (start_pos == std::string::npos)
			return;
		str.replace(start_pos, from.length(), to);
	} while (true == true);
}

int main(int argc, char** argv) //	Enter the file paths of Quandle and Topology separated by space
{
	std::cout << "Hello World!\n";
    string QuanFile = argv[1];
    string TopoFile = argv[2];

	int QOrder = 0;
	std::vector<ContinuQuanTopology> finalQTopList;
	std::vector<std::vector<std::vector<int>>> QArray;
	std::vector<std::vector<int>> QTable;
	std::vector<int> QRow;

	// Load Quandle File
	std::ifstream ifs(QuanFile);
	std::string line;
	while (std::getline(ifs, line))
	{
		if ((line.empty() == true) || (line.front() == '#'))
		{
			continue;
		}
		std::string delim = "],";
		std::vector<std::string> quandleStr;
		auto start = 0U;
		auto end = line.find(delim);
		do {
			std::string tempStr;
			tempStr = line.substr(start, end - start);
			if (tempStr.empty() == false) {
				quandleStr.push_back(tempStr);
			}
			start = end + delim.length();
			end = line.find(delim, start);
		} while (end != std::string::npos);

		std::string tempStr = line.substr(start, end);
		if (tempStr.empty() == false) {
			quandleStr.push_back(line.substr(start, end));
		}

		for (int i = 0; i < quandleStr.size(); i++) {
			StrReplace(quandleStr[i], "[", "");
			StrReplace(quandleStr[i], "]", "");
			delim = ",";
			auto start = 0U;
			auto end = quandleStr[i].find(delim);
			std::vector<std::string> items;
			do {
				std::string tempStr;
				tempStr = quandleStr[i].substr(start, end - start);
				if (tempStr.empty() == false) items.push_back(tempStr);
				start = end + delim.length();
				end = quandleStr[i].find(delim, start);
			} while (end != std::string::npos);

			tempStr = quandleStr[i].substr(start, end);
			if (tempStr.empty() == false) items.push_back(tempStr);

			if (items.size() < 2)
			{
				continue;
			}
			QOrder = items.size();
			QRow = std::vector<int>(QOrder);
			for (int j = 0; j < items.size(); j++)
			{
				QRow[j] = std::stoi(items[j]);
			}
			QTable.push_back(QRow);
		}
		QArray.push_back(QTable);
		QTable.clear();

	}


	// Load Topology File
	std::ifstream ifs2(TopoFile);
//	std::string line;
	std::vector<std::vector<std::vector<int>>> topolList;

	while (std::getline(ifs2, line))
	{
		if ((line.empty() == true) || (line.front() == '#'))
		{
			continue;
		}
		std::string delim = "},";
		std::vector<std::string> topolStr;
		auto start = 0U;
		auto end = line.find(delim);
		do {
			std::string tempStr;
			tempStr = line.substr(start, end - start);
			if (tempStr.empty() == false) {
				topolStr.push_back(tempStr);
			}
			start = end + delim.length();
			end = line.find(delim, start);
		} while (end != std::string::npos);

		std::string tempStr = line.substr(start, end);
		if (tempStr.empty() == false) {
			topolStr.push_back(line.substr(start, end));
		}

		vector<vector<int>> topol;//(topolStr.size());
		for (int i = 0; i < topolStr.size(); i++) {
			StrReplace(topolStr[i], "{", "");
			StrReplace(topolStr[i], "}", "");
			delim = ",";
			auto start = 0U;
			auto end = topolStr[i].find(delim);
			std::vector<std::string> items;
			int loopCount = 0;
			while (end != std::string::npos) {
				std::string tempStr;
				loopCount += 1;
				tempStr = topolStr[i].substr(start, end - start);
				if (tempStr.empty() == false) items.push_back(tempStr);
				start = end + delim.length();
				end = topolStr[i].find(delim, start);
			} //while (end != std::string::npos);

			tempStr = topolStr[i].substr(start, end);
			if (tempStr.empty() == false) items.push_back(tempStr);


//			if (items.size() < 2)
	//		{
		//		continue;
			//}

			std::vector<int> subset(items.size());// = ;//(QOrder);
			for (int j = 0; j < items.size(); j++)
			{
				subset[j] = std::stoi(items[j]);
			}
			topol.push_back(subset);
		}
		topolList.push_back(topol);
		topol.clear();

	}

//	std::vector<std::vector<std::vector<Intpair>>> PreImMapArray2;
	// PreImMapArray2: Level 1 - Quandle ID, 2 - Order number, 3 - array of integer rows each containing a preimage pair
	QuandleTopologies* QTop1 = new QuandleTopologies(QOrder);
//	QTop1->PreparePreImage2(PreImMapArray2, QArray); // Prepare PreImage

//	Topologies* topo1 = new Topologies(QOrder);
	//topo1->GenerateTopologies(topolList);
	finalQTopList = QTop1->GenerateQuandleTopologies( topolList, QArray);
	delete QTop1;

	std::vector<std::wstring> output(finalQTopList.size());

	for (int i = 0; i < finalQTopList.size(); i++)
	{
		std::wstring outputline = L"Quandle: {";
		for (int j = 0; j < finalQTopList[i].QTable.size(); j++)
		{
			std::vector<int> qRow;
			qRow = finalQTopList[i].QTable[j];
			//JAVA TO C++ CONVERTER TODO TASK: There is no C++ equivalent to 'toString':
			for (int x : qRow) {
				outputline += std::to_wstring(x);
			}

			if (j == finalQTopList[i].QTable.size() - 1)
			{
				outputline = outputline + L"}  Topology: {";
			}
			else
			{
				outputline = outputline + L", ";
			}
		}
		for (int j = 0; j < finalQTopList[i].Topol.size(); j++)
		{
			std::vector<int> tempTop;
			tempTop = finalQTopList[i].Topol[j];
			//JAVA TO C++ CONVERTER TODO TASK: There is no C++ equivalent to 'toString':
			for (int x : tempTop) {
				outputline += std::to_wstring(x);
			}
			if (j == finalQTopList[i].Topol.size() - 1)
			{
				outputline = outputline + L"}";
			}
			else
			{
				outputline = outputline + L", ";
			}
		}
        outputline = outputline + L"  --  POSets: ";
        //std::wstring wstr = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(finalQTopList[i].poset);
        //outputline = outputline + wstr;


		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        //std::string str = converter.to_bytes(finalQTopList[i].poset);
        std::wstring wstr = converter.from_bytes(finalQTopList[i].poset);
        outputline = outputline + wstr;

        output[i] = outputline + L"  --  \n";

	}
	//Arrays::sort(output);
	std::sort(output.begin(), output.end());



	string myFilename = string(OUTFILE_PATH) + "QuandleTopology" + to_string(QOrder) + ".txt";
    ofstream myfileOut (myFilename);
    if (myfileOut.is_open())
    {
        myfileOut << "Quandle Topologies Count: " << output.size() << endl;
        for (int i = 0; i < output.size(); i++)
        {
            std::string outStr(output[i].begin(), output[i].end());
            myfileOut << outStr;
        }
        myfileOut.close();
    }

    else cout << "Unable to open file";

	cout << "Quandle Topologies Count: " << output.size() << endl;

	return 1;

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
