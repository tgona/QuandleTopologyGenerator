#include <string>
#include <vector>
#include <algorithm>
#pragma once

using Intpair = std::pair<int, int>;
using namespace std;

struct ContinuQuanTopology
	{
		std::vector<std::vector<int>> QTable = std::vector<std::vector<int>>();
		std::vector<std::vector<int>> Topol = std::vector<std::vector<int>>();
		string poset;
	};


struct HasseRelation
	{
		vector<int> TopIndex = vector<int>();
		std::vector<Intpair> HasseTab = std::vector<Intpair>();
		std::vector<std::vector<Intpair>> HasseTabVar = std::vector<std::vector<Intpair>>();
	};


struct HasseQuandleContinuous
	{
		vector<int> TopIndex = vector<int>();
		std::vector<Intpair> HasseTab = std::vector<Intpair>();
		int QTabIndex = 0;
	};
