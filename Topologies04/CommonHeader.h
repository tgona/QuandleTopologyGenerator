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
	};
