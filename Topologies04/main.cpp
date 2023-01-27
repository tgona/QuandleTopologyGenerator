// ConsoleApplication6.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//*****************************************************************************************
/// This program generates topologies of a specific order and downloads to the local folder.
/// The only argument is an integer for the order.
//*****************************************************************************************

#include "Topologies.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <sstream>
#define OUTFILE_PATH "C:\\Tushar\\"

using namespace std;

void StrReplace(std::string& str, const std::string& from, const std::string& to) {
	do {
		size_t start_pos = str.find(from);
		if (start_pos == std::string::npos)
			return;
		str.replace(start_pos, from.length(), to);
	} while (true == true);
}

int main(int argc, char **argv ) //Enter Quandle size as integer
{
	std::cout << "Hello World!\n";
	int QOrder = atoi(argv[1]);
//	int threadCount = atoi(argv[2]);


	std::vector<std::vector<std::vector<int>>> topolList;

	Topologies* topo1 = new Topologies(QOrder);
	topo1->GenerateTopologies(topolList);

    cout << "topoList size" << topolList.size() << endl;

	delete topo1;
	//JAVA TO C++ CONVERTER TODO TASK: A 'delete file' statement was not added since file was passed to a method or constructor. Handle memory management manually.



	std::vector<std::wstring> output(topolList.size());

	for (int i = 0; i < topolList.size(); i++)
	{
	    vector<vector<int>> topol = topolList[i];
		std::wstring outputline = L"{";
		std::wstring commaStr = L",";

        for (int j = 0; j < topol.size(); j++)
		{
			std::vector<int> subSet = topol[j];
            outputline += L"{";;

			for (int k = 0; k < subSet.size(); k++) {
                if (k == 0) outputline += std::to_wstring(subSet[k]);
                else outputline = outputline + commaStr + std::to_wstring(subSet[k]);
			}

	/*		for (int x : subSet) {
                if (x == 1) outputline += std::to_wstring(x);
                else outputline = outputline + commaStr + std::to_wstring(x);
			}*/

			if (j == topol.size() - 1)
			{
				outputline = outputline + L"}";
			}
			else
			{
				outputline = outputline + L"}, ";
			}
		}





		output[i] = outputline + L"}\n";
		outputline = L"";

	}
	//Arrays::sort(output);
	std::sort(output.begin(), output.end());

  string myFilename = string(OUTFILE_PATH) + "Topology" + to_string(QOrder) + ".txt";
  ofstream myfileOut (myFilename);
  if (myfileOut.is_open())
  {

	for (int i = 0; i < output.size(); i++)
	{
    std::string outStr(output[i].begin(), output[i].end());
		 myfileOut << outStr;
	}
	myfileOut.close();

  }

  else cout << "Unable to open file";






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
