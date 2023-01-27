#include <vector>
#include <pthread.h>
#pragma once

using namespace std;

class Topologies
{
public:
	int QOrder;
	int ThreadCount;







	Topologies(int qOrder);

	virtual void GenerateTopologies(std::vector<std::vector<std::vector<int>>>& topolList);

	static void *MakeTopoParallel(void *threadarg);


	static bool checkAxioms(std::vector<std::vector<int>>& topol, int qOrder);

	static void prepTopNextLevel (std::vector<std::vector<int>>& topol, std::vector<std::vector<std::vector<int>>>& topolList,
                                  std::vector<std::vector<std::vector<int>>>& listSubsetGrp, std::vector<int>& topElement,
                                  int nextLevel, int qOrder, int threadID);

    static int getSubsetIndex(std::vector<std::vector<int>>& listSubset, std::vector<int>& intersection);

    static bool findAddTopology(std::vector<int>& topol);

    static void addSubsetsFromMinBasis(std::vector<int>& newTopol, int newTerm, std::vector<std::vector<std::vector<int>>>& minBasis, bool topReached);

    static void addNextLevelTopo(int subsetFrom, int subsetTo, std::vector<int>& newTop, std::vector<std::vector<std::vector<int>>>& minBasis);

};



