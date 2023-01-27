#define _CRT_SECURE_NO_WARNINGS
#include "Topologies.h"
#include "Combination.h"
#include <math.h>
#include <ctime>
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 5


using namespace std;

Topologies::Topologies(int qOrder)
{
	QOrder = qOrder;
//	ThreadCount = threadCount;
}

	struct thread_data {
    int  thread_id;
    int qOrder;
    vector<vector<vector<int>>> listSubsetGrp;
    vector<int> topElement;
    vector<vector<vector<int>>> topolList;
    int startNum;
    int endNum;
};
    struct topIndexStruc {
      int indexNum;
      vector<int> topol;
      bool operator<(const topIndexStruc & other) const
      {
          return indexNum < other.indexNum;
      }
      bool operator==(const topIndexStruc & other) const
      {
          return indexNum == other.indexNum;
      }
    };

    std::vector<topIndexStruc> topIndexList;

template <class T>
void my_print(std::vector<T> input)
{
    for (typename std::vector<T>::iterator pp = input.begin(); pp != input.end(); ++pp)
    {
        std::cout << *pp << "\n";
    }
}

void Topologies::GenerateTopologies(std::vector<std::vector<std::vector<int>>>& topolList)
{

	std::vector<std::vector<std::vector<int>>> listSubsetGrp; //Subsets grouped by element order

	std::vector<int> topElement (QOrder);
	int count;
	switch (QOrder) {
	case 3:
		count = 19;
		break;
	case 4:
		count = 219;
		break;
	case 5:
		count = 4231;
		break;
	case 6:
		count = 130023;
		break;
	case 7:
		count = 6129859;
		break;
	}
	topolList.reserve(count);
	//Prepare all subsets
	std::vector<std::vector<int>> listSubset; //Subsets of all order sizes



	for (int i = 1; i <= QOrder; i++)
	{

		Combination* c = new Combination(QOrder, i, false);
		while (c->hasNext())
		{
			std::vector<int> a = c->next();
			if (i != QOrder)
			{
				listSubset.push_back(a);
			}
			else
			{
				for (int j = 0; j < a.size(); j++)
				{
					topElement[j] = a[j];
				}
			}
			//c++;
		}
		//if (i < QOrder) listSubsetGrp.push_back(listSubset);
		delete c;
	}

//	int minBasis[listSubset.size()][listSubset.size()][3];
    vector<vector<vector<int>>> minBasis;
	//Minimum basis for each pair of subsets. Index 0 and 1 form any union and intersection subsets.
	// Value of 999 in Index 2 represents the union is a top element.

    vector<vector<int>> minBasisTop; //Min Basis entries in standard topology format

    /// Initialize Array
    std::vector<int> subsets = {-1,-1,-1};
    vector<vector<int>> subsetRows;
    for (int i = 0; i < listSubset.size(); i++)
        {
            subsetRows.push_back(subsets);
        }
    for (int j = 0; j < listSubset.size(); j++)
    {
        minBasis.push_back(subsetRows);
    }


// Prepare minimum basis for each pair
    for (int i = 0; i < listSubset.size() - 1; i++)
	{
	    for (int j = i + 1; j < listSubset.size(); j++)
        {
            std::vector<int> intersection;
            int counter = 0;

        //Intersection prepare
            for (int k = 0; k < listSubset[i].size(); k++)
            {
                for (int l = 0; l < listSubset[j].size(); l++)
                {
                    if (listSubset[i][k] == listSubset[j][l])
                    {
                        intersection.push_back(listSubset[i][k]);
                        break;
                    }
                }
            }
            if (intersection.size() > 0 and intersection != listSubset[i] and intersection != listSubset[j])
            {
              minBasis[i][j][counter] = getSubsetIndex(listSubset, intersection);
              counter += 1;
            }


            //Union prepare
            std::vector<int> array3(listSubset[i].size() + listSubset[j].size());
            for (int k = 0; k < listSubset[i].size(); k++)
            {
                array3[k] = listSubset[i][k];
            }
            for (int k = 0; k < listSubset[j].size(); k++)
            {
                array3[k + listSubset[i].size()] = listSubset[j][k];
            }
            std::sort(array3.begin(), array3.end());
            //Arrays::sort(array3);
            std::vector<int> unionKeyword;

            int preNum = 0;
            for (int k = 0; k < array3.size(); k++)
            {
                if (array3[k] != preNum)
                {
                    unionKeyword.push_back(array3[k]);
                    preNum = array3[k];
                }

            }

            if (unionKeyword.size() > 0 && unionKeyword.size() != QOrder &&
                unionKeyword != listSubset[i] && unionKeyword != listSubset[j])
                {
                  minBasis[i][j][counter] = getSubsetIndex(listSubset, unionKeyword);
                  counter += 1;
                }

            if (unionKeyword.size() == QOrder) minBasis[i][j][2] = 999;

            std::vector<int> localTop;
/// Add the min basis to the Topology Index list
            localTop.push_back(i);
            localTop.push_back(j);
            for (int k=0; k<counter; k++){
                localTop.push_back(minBasis[i][j][k]);
            }
            std::sort(localTop.begin(), localTop.end());  //Sort topology by integer index ascending
            findAddTopology(localTop);
            minBasisTop.push_back(localTop);
        }
	}

	/*
	for (int p=0; p < minBasisTop.size(); p++) {
        for (int q=0; q < minBasisTop[p].size(); q++){
        cout << minBasisTop[p][q] << ",";
        }
        cout << endl;
	} */

	auto it = minBasisTop[0].begin();
	std::vector<int> newTop;

//  Loop through minimum basis topologies and recursively build all topologies
	for (int i = 0; i < minBasisTop.size(); i++){

        Topologies::addNextLevelTopo(0, listSubset.size(), minBasisTop[i], minBasis);

/*        for (int j = 0; j < listSubset.size(); j++){
            newTop = minBasisTop[i];

            if (find(newTop.begin(), newTop.end(), j) == newTop.end()){
            //Element is not part of current list. Add new subsets for each min basis combination for element j
                addSubsetsFromMinBasis(newTopol, newElements[i], *p_array, minBasisSize, lclTopReached);
//                for (int k=0;k<minBasisTop[i])
                if (findAddTopology(localTop) == true) {

                }
            }

            else continue;
        }*/
	}

    time_t now = time(0);
    tm* local_time = localtime(&now);

    //char* dt = local_time->tm_hour();
    std::cout << " Total Topologies before T0 filter: " << topIndexList.size() << "  Time: " << local_time->tm_hour << ":" << local_time->tm_min << ":" << local_time->tm_sec << std::endl;


	for (int i=0; i<topIndexList.size(); i++){
        vector<vector<int>> localTop;
        for (int j=0; j<topIndexList[i].topol.size(); j++){
            localTop.push_back(listSubset[topIndexList[i].topol[j]]);
        }
        localTop.push_back(topElement);
        if (Topologies::checkAxioms(localTop, QOrder))
        {
            topolList.push_back(localTop);
            if (topolList.size() % 1000 == 0)
            {
                time_t now = time(0);
                tm* local_time = localtime(&now);

                //char* dt = local_time->tm_hour();
                std::cout << " Topologies: " << topolList.size() << "  Time: " << local_time->tm_hour << ":" << local_time->tm_min << ":" << local_time->tm_sec << std::endl;
            }
        }
	}
}

void Topologies::addNextLevelTopo(int subsetFrom, int subsetTo, std::vector<int>& newTop, std::vector<std::vector<std::vector<int>>>& minBasis){
        std::vector<int> newTopol;
        bool lclTopReached;

        for (int j = subsetFrom; j < subsetTo; j++){
            newTopol = newTop;

            if (find(newTopol.begin(), newTopol.end(), j) == newTopol.end()){
            //Element is not part of current list. Add new subsets for each min basis combination for element j recursively
            //until all dependent subsets are added.
                Topologies::addSubsetsFromMinBasis(newTopol, j, minBasis, lclTopReached);
                std::sort(newTopol.begin(), newTopol.end());
                //If topology is new and top element is not reached, keep adding next level to try generate next topology.
                if (Topologies::findAddTopology(newTopol) == true && j < subsetTo - 1 && lclTopReached == false) {
                    Topologies::addNextLevelTopo(j+1, subsetTo, newTopol, minBasis);
                }
            }
        }
}

int Topologies::getSubsetIndex(std::vector<std::vector<int>>& listSubset, std::vector<int>& intersection)
{
    for (int i=0; i<listSubset.size();i++)
    {
        if (listSubset[i] == intersection)
        {
            return i;
        }
    }
}


template<typename Iterator>
Iterator search(Iterator initial, Iterator final, const typename Iterator::value_type& value) {

//BEHAVIOR: If index is outside range, return = Iterator.end().
// If multiple matches found (multiple with same index value), one of them will be randomly returned. If no match, the
// immediate next will be returned.

  if(value < *initial) {
    // bail out immediately
    return final;
  }

  while(initial != final) {
    auto mid = std::next(initial, std::distance(initial, final) / 2);
    if(*mid == value) {
      return mid;
    } else if(*mid < value) {
      initial = std::next(mid);
    } else {
      final = mid; //std::prev(mid);
    }
  }
  return final;
}

bool Topologies::findAddTopology(std::vector<int>& topol){

    std::vector<int> localTopol = topol;

    topIndexStruc localTopIndx;
    bool pushback = false;
    auto it = topIndexList.begin();

    localTopIndx.indexNum = 0;
    //Calculate sum of first 10 terms to prepare the sort index
    for (int i=0; i<localTopol.size(); i++){
      if (i==10) break;
      localTopIndx.indexNum += localTopol[i];
    }
    localTopIndx.topol = localTopol;

    if (topIndexList.size() > 0){
       it = search(topIndexList.begin(), topIndexList.end(), localTopIndx);//.indexNum);
       if ((*it).indexNum == localTopIndx.indexNum){
// Matched index number. Navigate backwards to the first topology of this index number.
         while (it != topIndexList.begin()){
           --it;
           if ((*it).indexNum != localTopIndx.indexNum){
             it++;
             break;
           }
         }
// From that row, keep navigating forward to find a complete topology match.
         while (it != topIndexList.end()){
           if ((*it).indexNum == localTopIndx.indexNum){
             if ((*it).topol == localTopIndx.topol){
                 return false; //match found. Topology not added.
             }
             else it++;
           }
           else {//no more rows with matching index. Use current iterator position to insert the topology.
             break;
           }
         }
       }
       else { //No matching index found. Check if the search is outside range or there is only one topology in the list.
         if (localTopIndx.indexNum > topIndexList[topIndexList.size() - 1].indexNum) pushback = true; //(*topIndexList.end()).indexNum) pushback = true;
         else if (localTopIndx.indexNum < topIndexList[0].indexNum) it = topIndexList.begin();//(*topIndexList.begin()).indexNum) it = topIndexList.begin();
       }
    }
    else pushback = true;//Topology list empty. Push back

    if (pushback == true) topIndexList.push_back(localTopIndx);
    else it = topIndexList.insert(it, localTopIndx);

    if (topIndexList.size() % 1000 == 0)
    {
        time_t now = time(0);
        tm* local_time = localtime(&now);

        //char* dt = local_time->tm_hour();
        std::cout << " Topologies: " << topIndexList.size() << "  Time: " << local_time->tm_hour << ":" << local_time->tm_min << ":" << local_time->tm_sec << std::endl;
    }


    return true;//Successfully inserted topology
}

void Topologies::addSubsetsFromMinBasis(std::vector<int>& newTopol, int newTerm, std::vector<std::vector<std::vector<int>>>& minBasis, bool topReached){

    vector<int> newElements;
    int first, second;
    bool lclTopReached = false;

//    if (find(newTopol.begin(), newTopol.end(), j) == newTopol.end()){//Make sure element doesn't already exist

      for (int i=0; i<newTopol.size(); i++){
        if (newTerm < newTopol[i]) {
            first = newTerm;
            second = newTopol[i];
        }
        else{
            first = newTopol[i];
            second = newTerm;
        }
        if ((minBasis[first][second][0] != -1) &&
            find(newTopol.begin(), newTopol.end(), minBasis[first][second][0]) == newTopol.end() &&
            find(newElements.begin(), newElements.end(), minBasis[first][second][0]) == newElements.end())
                newElements.push_back(minBasis[first][second][0]);

        if ((minBasis[first][second][1] != -1) &&
            find(newTopol.begin(), newTopol.end(), minBasis[first][second][1]) == newTopol.end() &&
            find(newElements.begin(), newElements.end(), minBasis[first][second][1]) == newElements.end())
                newElements.push_back(minBasis[first][second][1]);

        if (minBasis[first][second][2] == 999) topReached = true;
      }

      newTopol.push_back(newTerm);

      if (newElements.size() > 0) {
        for (int i=0; i<newElements.size(); i++) {
            if (find(newTopol.begin(), newTopol.end(), newElements[i]) == newTopol.end()){
                addSubsetsFromMinBasis(newTopol, newElements[i], minBasis, lclTopReached);
                if (lclTopReached == true) topReached = true;
            }
        }
      }
  //  }
    //(*p_array)[][][]

}


void *Topologies::MakeTopoParallel(void *threadarg){
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;


	//Prepare indexes for all combinations of subsets
	std::vector<std::vector<int>> listIndx;

	for (int i = my_data->startNum; i <= my_data->endNum; i++)
	{

		Combination* c = new Combination(my_data->qOrder, i, true);
		while (c->hasNext())
		{
			vector<int> a = c->next();
			vector<std::vector<int>> topol;
			vector<int> indxArray(a.size());
			vector<int> subsetArray;
			indxArray = a;
			topol.reserve(indxArray.size() + 1);
			//indxArray =  Arrays.asList(listIndx.get(i));
			for (int j = 0; j < indxArray.size(); j++)
			{
				topol.push_back(my_data->listSubsetGrp[0][indxArray[j]]);
			}

			int nextLevel = 2;

            Topologies::prepTopNextLevel (topol, my_data->topolList, my_data->listSubsetGrp, my_data->topElement,
                                          nextLevel, my_data->qOrder, my_data->thread_id);


			//topol.push_back(my_data->topElement);







/*
	    //only consider 2 to SIZE - 1.
		// top element (i=SIZE) added at the end. i = 1 means topology will have
		// 2 sets only. This can be excluded as it is a non-T0 topology.
		Combination* c = new Combination(my_data->listSubset.size(), i, true);
		while (c->hasNext())
		{
			vector<int> a = c->next();
			vector<std::vector<int>> topol;
			vector<int> indxArray(a.size());
			vector<int> subsetArray;
			indxArray = a;
			topol.reserve(indxArray.size() + 1);
			//indxArray =  Arrays.asList(listIndx.get(i));
			for (int j = 0; j < indxArray.size(); j++)
			{
				topol.push_back(my_data->listSubset[indxArray[j]]);
			}
			topol.push_back(my_data->topElement);
		if (Topologies::checkAxioms(topol, my_data->qOrder))
			{
				my_data->topolList.push_back(topol);
				if (my_data->topolList.size() % 100 == 0)
				{
					time_t now = time(0);
					tm* local_time = localtime(&now);

					//char* dt = local_time->tm_hour();
					std::cout << " Thread: "<< my_data->thread_id << " Topologies: " << my_data->topolList.size() << "  Time: " << local_time->tm_hour << ":" << local_time->tm_min << ":" << local_time->tm_sec << std::endl;
				}
			}
			//listIndx.get(i);

			//c++; */
		}

		delete c;
	}
	cout << "Thread: " << my_data->thread_id << "  Topol count: " << my_data->topolList.size() << endl;
	//Add top element to topology
	int test = 0;
	pthread_exit(NULL);

}




bool Topologies::checkAxioms(std::vector<std::vector<int>>& topol, int qOrder)
{

	//T_0 topology check: For each pair of numbers in each subset, check if they exist without each other in any
	// other subsets. Then it is a T_0 topology and should be considered. Even if one such pair fails this test
	// it's a non T_0 topology and should be excluded.
	int maxPairCount = qOrder * (qOrder - 1) / 2;
	vector<vector<int>> okPairs;
	okPairs.reserve(maxPairCount);
	vector<int> pair_1;
	//pair_1.reserve(2);
	bool all_T0s_done = false;

	for (int i = 0; i < topol.size(); i++)
	{
	    if (all_T0s_done == true) break;
		std::vector<int> subset = topol[i];
		if (subset.size() > 1)
		{
			Combination* c2 = new Combination(subset.size(), 2, true);
			while (c2->hasNext() && all_T0s_done == false)
			{
				bool comb_fail = true;
				std::vector<int> combo_indx = c2->next();
				int num1, num2;
				if (subset[combo_indx[0]] < subset[combo_indx[1]]) {
				  num1 = subset[combo_indx[0]];
				  num2 = subset[combo_indx[1]]; }
				else {
                  num1 = subset[combo_indx[1]];
                  num2 = subset[combo_indx[0]];
				}

                bool goodPair = false;
				for (int p = 0; p < okPairs.size(); p++ ){
                        vector<int> pair2 = okPairs[p];
                //cout << okPairs.size();
                    if (num1 == pair2[0] and num2 == pair2[1]) {
                        goodPair = true;
                        break;
                    }
				}

				if (goodPair == true) continue;

				for (int j = 0; j < topol.size(); j++)
				{
					if (i != j)
					{
						int match_count = 0;
						for (int k = 0; k < topol[j].size(); k++)
						{
							if (topol[j][k] == num1 || topol[j][k] == num2)
							{
								match_count = match_count + 1;
								if (match_count > 1)
									break;
							}
						}
						if (match_count == 1)
						{
						    pair_1.clear();
							comb_fail = false;
							pair_1.push_back(num1);
							pair_1.push_back(num2);
							okPairs.push_back(pair_1);
					//		cout << "okPairs size: " << okPairs.size() << endl;
							if (okPairs.size() == maxPairCount) all_T0s_done = true;
							break;
						}
					}
				}
				if (comb_fail == true)
				{
					delete c2;
//					delete c;
					return false;
				}
				//c2++;
			}


			delete c2;
		}

	}
/*
	//Prepare indexes for all Nc2 combinations of subsets of the topology
	std::vector<std::vector<int>> listIndx1;
	Combination* c = new Combination(topol.size(), 2, true);
	while (c->hasNext())
	{
		std::vector<int> a1 = c->next();
		std::vector<int> array1(topol[a1[0]].size());
		array1 = topol[a1[0]];
		std::vector<int> array2(topol[a1[1]].size());
		array2 = topol[a1[1]];
		std::vector<int> intersection;
		intersection.reserve(int(0.1 * array1.size()));
		bool match = false;





	//Axiom 2 - Intersection check
		for (int i = 0; i < array1.size(); i++)
		{
			for (int j = 0; j < array2.size(); j++)
			{
				if (array1[i] == array2[j])
				{
					intersection.push_back(array1[i]);
					break;
				}
			}
		}
		if (intersection.size() > 0)
		{
			// Check if the intersection subset exists in the topology
			for (int i = 0; i < topol.size(); i++)
			{
				if (intersection.size() == topol[i].size())
				{
					match = true;
					for (int j = 0; j < intersection.size(); j++)
					{
						if (intersection[j] != topol[i][j])
						{
							//if (intersection.toString() == (topol.get(i).toString())) {
							match = false;
							break;
						}
					}
					if (match == true)
					{
						break;
					}
				}
                else if (intersection.size() < topol[i].size()) break;
			}
			if (!match)
			{
				delete c;
				return false;
			}
		}
		else
		{
			match = true; //Intersection is null
		}
		//listIndx1.add(a1);


	//Axiom 3 - Union check
		std::vector<int> array3(array1.size() + array2.size());
		for (int i = 0; i < array1.size(); i++)
		{
			array3[i] = array1[i];
		}
		for (int i = 0; i < array2.size(); i++)
		{
			array3[i + array1.size()] = array2[i];
		}
		std::sort(array3.begin(), array3.end());
		//Arrays::sort(array3);
		std::vector<int> union_Keyword;
		union_Keyword.reserve(array3.size());
		int preNum = 0;
		for (int i = 0; i < array3.size(); i++)
		{
			if (array3[i] != preNum)
			{
				union_Keyword.push_back(array3[i]);
				preNum = array3[i];
			}

		}
		if (union_Keyword.size() > 0 and union_Keyword.size() != qOrder)
		{
			match = false;
			// Check if the union subset exists in the topology
			for (int i = 0; i < topol.size(); i++)
			{
				if (union_Keyword.size() == topol[i].size())
				{
					match = true;
					for (int j = 0; j < union_Keyword.size(); j++)
					{
						if (union_Keyword[j] != topol[i][j])
						{
							match = false;
							break;
						}
					}
					if (match == true)
					{
						break;
					}
				}
				else if (union_Keyword.size() < topol[i].size()) break;
			}
			if (!match)
			{
				delete c;
				return false;
			}
		}
	//	c++;
	} */

//	delete c;
	return true;

}


void Topologies::prepTopNextLevel (std::vector<std::vector<int>>& topol, std::vector<std::vector<std::vector<int>>>& topolList,
                                   std::vector<std::vector<std::vector<int>>>& listSubsetGrp, std::vector<int>& topElement,
                                   int nextLevel, int qOrder, int threadID){

    std::vector<std::vector<int>> currTopol = topol;

    for (int i = 1; i <= listSubsetGrp[nextLevel - 1].size(); i++)
        {
            if (topol.size() >= 10 and i == 4){
                int pp = 0;
            }

            Combination* c = new Combination(listSubsetGrp[nextLevel - 1].size(), i, true);
            while (c->hasNext())
            {
                vector<int> a = c->next();
                //vector<std::vector<int>> topol;
                vector<int> indxArray(a.size());
                vector<int> subsetArray;
                indxArray = a;
                std::vector<std::vector<int>> currSubsets;
                //topol.reserve(indxArray.size() + 1);
                //indxArray =  Arrays.asList(listIndx.get(i));

                currTopol = topol;

                for (int j = 0; j < indxArray.size(); j++)
                {
                    currTopol.push_back(listSubsetGrp[nextLevel - 1][indxArray[j]]);
                    currSubsets.push_back(listSubsetGrp[nextLevel - 1][indxArray[j]]);
                }


// quickly check if the intersection exists in previous level sets in the topology. Otherwise, skip this combination.
                bool validCombo = true;
                for (int k = 0; k < currSubsets.size() - 1; k++){
                    for (int l = k + 1; l < currSubsets.size(); l++){
                        std::vector<int> array1 = currSubsets[k];
                        std::vector<int> array2 = currSubsets[l];
                        std::vector<int> intersection;
                        for (int m = 0; m < array1.size(); m++)
                        {
                            for (int n = 0; n < array2.size(); n++)
                            {
                                if (array1[m] == array2[n])
                                {
                                    intersection.push_back(array1[m]);
                                    break;
                                }
                            }
                        }
                        if (intersection.size() > 0)
                        {
                            bool match = false;
                            // Check if the intersection subset exists in the topology
                            for (int m = 0; m < topol.size(); m++)
                            {
                                if (intersection.size() == topol[m].size())
                                {
                                    match = true;
                                    for (int n = 0; n < intersection.size(); n++)
                                    {
                                        if (intersection[n] != topol[m][n])
                                        {
                                            //if (intersection.toString() == (topol.get(i).toString())) {
                                            match = false;
                                            break;
                                        }
                                    }
                                    if (match == true)
                                    {
                                        break;
                                    }
                                }
                                else if (intersection.size() < topol[m].size()) break;
                            }
                            if (!match)
                            {
                                validCombo = false;
                                break;
                            }
                        }

                    }
                    if (validCombo == false) break;
                }
                if (validCombo == false) continue;



                if (nextLevel == qOrder - 1)
                  { //Last level. Wrap up and check all axioms.
                      currTopol.push_back(topElement);
                      if (Topologies::checkAxioms(currTopol, qOrder))
                        {
                            topolList.push_back(currTopol);
                            if (topolList.size() % 100 == 0)
                            {
                                time_t now = time(0);
                                tm* local_time = localtime(&now);

                                //char* dt = local_time->tm_hour();
                                std::cout << " Thread: "<< threadID << " Topologies: " << topolList.size() << "  Time: " << local_time->tm_hour << ":" << local_time->tm_min << ":" << local_time->tm_sec << std::endl;
                            }
                        }
                }
                else Topologies::prepTopNextLevel (currTopol, topolList, listSubsetGrp, topElement, nextLevel + 1, qOrder, threadID);

         }
    }

}
