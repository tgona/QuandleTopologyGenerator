#include <string>
#include <vector>
#include <iostream>
#include "CommonHeader.h"
#include "Combination.h"
#pragma once
class QuandleTopologies
{

    public:
        int QOrder = 0;
        QuandleTopologies(int qOrder);
        //void PreparePreImage2(std::vector<std::vector<std::vector<Intpair>>>& preImMapArray2, std::vector<std::vector<std::vector<int>>>& QArray);
        //static void PreImSetMatchedProductPairSet(std::vector<Intpair>& preImSets, std::vector<std::vector<Intpair>>& topoProduct, int startIndx, bool& matchResult);
        void PrepareHasseTable(std::vector<HasseRelation>& HasseRelTab, std::vector<std::vector<std::vector<int>>>& topolList, vector<vector<int>>& myQOrderCombo);
        std::vector<ContinuQuanTopology> GenerateQuandleTopologies(std::vector<std::vector<std::vector<int>>>& topolList, std::vector<std::vector<std::vector<int>>>& qArray);
        //static void main(std::vector<std::wstring> &args);
        static void *QTopGenerateParallel(void *threadarg);
        //bool compareIntPairs(Intpair& i1, Intpair& i2);

};

