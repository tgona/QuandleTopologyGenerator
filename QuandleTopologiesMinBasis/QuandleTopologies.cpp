#include "QuandleTopologies.h"
#include <pthread.h>
#include <math.h>
#include <ctime>
#define NUM_THREADS 1

using namespace std;

QuandleTopologies::QuandleTopologies(int qOrder)
{
	QOrder = qOrder;
}

	struct thread_data
	{
        int  thread_id;
        int QOrder;
//        vector<vector<vector<Intpair>>> PreImMapArray;
//        vector<vector<vector<int>>> topolList;
        vector<HasseRelation> HasseRelTab;
        vector<vector<vector<int>>> qArray;
//        vector<ContinuQuanTopology> QTopList;
        vector<HasseQuandleContinuous> hQuanCont;
        vector<vector<int>> qOrderCombo;
        int startNum;
        int endNum;
    };

         // Compares two int pairs for ascending order.
     bool compareIntPairs(Intpair i1, Intpair i2)
     {
        if (i1.first < i2.first || (i1.first == i2.first && i1.second < i2.second)) return true;
            else
                return false;
        }

void QuandleTopologies::PrepareHasseTable(std::vector<HasseRelation>& HasseRelTab, std::vector<std::vector<std::vector<int>>>& topolList, vector<vector<int>>& myQOrderCombo)
{
        std::vector<int> QOrderList;
        std::vector<std::vector<int>> topol;

        bool matched = false;
        for (int i = 0; i < QOrder; i++) {
          QOrderList.push_back( i+1 );
        }

		for (int i = 0; i < topolList.size(); i++){
            topol = topolList[i];
            std::vector<std::vector<int>> minBasis;
            HasseRelation hasseRel;

            std::vector<int> QOList1 = QOrderList;

// Prepare minimum Basis
            for (int j = 0; j < topol.size(); j++){
                matched = false;
                for (int k = 0; k < topol[j].size(); k++){
                  for (int l = 0; l < QOList1.size(); l++){
                    if (QOList1[l] == topol[j][k]) {
                        QOList1.erase(QOList1.begin() + l);
                        matched = true;
                        break;
                    }
                  }

                }
                if (matched == true) {
                  minBasis.push_back(topol[j]);
                }
                if (QOList1.size() == 0){
                    break;
                }
            }

// Prepare Hasse Relationship Table
            Intpair intpair1;
            hasseRel.TopIndex.push_back(i);
            for (int j = 0; j < minBasis.size() - 1; j++){
              std::vector<int> currSet = minBasis[j];

              for (int k = j+1; k < minBasis.size(); k++){
                 int matchCount = 0;
                 if (minBasis[k].size() >= currSet.size()){
                    for (int l = 0; l < currSet.size(); l++){
                       for (int m = 0; m < minBasis[k].size(); m++){
                         if (currSet[l] == minBasis[k][m]){
                           matchCount += 1;
                           break;
                         }
                       }
                    }
                    if (matchCount == currSet.size()){
                      intpair1.first = j;
                      intpair1.second = k;
                      hasseRel.HasseTab.push_back(intpair1);
                    }

                 }
              }
            }

// Check if the Hasse Relation already exists and only add if it is new.
            if (hasseRel.HasseTab.size() != 0){
                bool newRelation = true;
                for (int k = 0; k < HasseRelTab.size(); k++){
                  if (HasseRelTab[k].HasseTab.size() == hasseRel.HasseTab.size()){
                     for (int l = 0; l < HasseRelTab[k].HasseTabVar.size(); l++){ //Check if it matches any variation of existing HasseTabs
                        if (HasseRelTab[k].HasseTabVar[l] == hasseRel.HasseTab){
                           // HasseRelTab[k].TopIndex.push_back(hasseRel.TopIndex[0]);
                            newRelation = false;
                            break;
                        }
                     }
                  }
                  if (newRelation == false) break;
                }
                if (newRelation == true){
                  //Prepare all variations of Hassetab
                  for (int k = 0; k < myQOrderCombo.size(); k++){
                    std::vector<Intpair> HasseTabVarRec;
                    for (int l = 0; l < hasseRel.HasseTab.size(); l++ ){
                      Intpair intpair1;
                      intpair1.first = myQOrderCombo[k][hasseRel.HasseTab[l].first] - 1;
                      intpair1.second = myQOrderCombo[k][hasseRel.HasseTab[l].second] - 1;
                      HasseTabVarRec.push_back(intpair1);
                    }
                    sort(HasseTabVarRec.begin(), HasseTabVarRec.end(), compareIntPairs);
                    hasseRel.HasseTabVar.push_back(HasseTabVarRec);
                  }
                  HasseRelTab.push_back(hasseRel);
                }
            }
		}

}



std::vector<ContinuQuanTopology> QuandleTopologies::GenerateQuandleTopologies(std::vector<std::vector<std::vector<int>>>& topolList, std::vector<std::vector<std::vector<int>>>& qArray)
{
    std::vector<HasseRelation> myHasseRelTab;
    vector<HasseQuandleContinuous> finalHQuanCont;
	std::vector<ContinuQuanTopology> finalQTopList;
    vector<vector<int>> myQOrderCombo;



    string strQorderCombo;
    strQorderCombo.resize(QOrder);
    for (int i = 0; i < QOrder; i++){
        strQorderCombo[i] = i + 1;
    }

// Prepare all permutations of the QOrder elements: For order n, will generate n! entries.
    do {
        vector<int> vec;

        for (int i = 0; i < strQorderCombo.size(); i++)
            {                                 // This converts the char into an int and pushes it into vec
            vec.push_back(strQorderCombo[i] + 0); //- '0');  // The digits will be in the same order as before
            }
        myQOrderCombo.push_back(vec);
        } while (next_permutation(strQorderCombo.begin(), strQorderCombo.end()));


    PrepareHasseTable(myHasseRelTab, topolList, myQOrderCombo);

// Create multiple threads and distribute work
   pthread_t threads[NUM_THREADS];
   struct thread_data td[NUM_THREADS];
   pthread_attr_t attr;
   void *status;
   int rc;
   int i;



   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   int lastNum = -1;

   for( i = 0; i < NUM_THREADS; i++ ) {
      cout <<"main() : creating thread, " << i << endl;
      td[i].thread_id = i;
      td[i].QOrder = QOrder;
      td[i].HasseRelTab = myHasseRelTab;
      td[i].qOrderCombo = myQOrderCombo;
//      td[i].topolList = topolList;
//      td[i].PreImMapArray = PreImMapArray;
      td[i].qArray = qArray;
      td[i].startNum = lastNum + 1;

      if (i == NUM_THREADS - 1) lastNum = td[i].endNum = myHasseRelTab.size() - 1;
      else lastNum = td[i].endNum = td[i].startNum + round(myHasseRelTab.size() / NUM_THREADS) - 1;

      rc = pthread_create(&threads[i], NULL, &QuandleTopologies::QTopGenerateParallel, (void *)&td[i]);

      if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }


   // free attribute and wait for the other threads
   pthread_attr_destroy(&attr);
   for( i = 0; i < NUM_THREADS; i++ ) {
      rc = pthread_join(threads[i], &status);
      if (rc) {
         cout << "Error:unable to join," << rc << endl;
         exit(-1);
      }
      cout << "Main: completed thread id :" << i ;
      cout << "  exiting with status :" << status << endl;
      finalHQuanCont.insert(finalHQuanCont.end(), td[i].hQuanCont.begin(), td[i].hQuanCont.end());
   }

    for( i = 0; i < finalHQuanCont.size(); i++ ) {
		ContinuQuanTopology conQTop;
		string poset;
		conQTop.QTable = qArray[finalHQuanCont[i].QTabIndex];
//		for (int j = 0; j < finalHQuanCont[i].TopIndex.size(); j++){
        int j = 0;
            conQTop.Topol = topolList[finalHQuanCont[i].TopIndex[j]];

            for (int k = 0; k < finalHQuanCont[i].HasseTab.size(); k++){
                poset += to_string(finalHQuanCont[i].HasseTab[k].first);
                poset += "<=";
                poset += to_string(finalHQuanCont[i].HasseTab[k].second);
                if (k < finalHQuanCont[i].HasseTab.size() - 1) poset += ", ";
            }
            conQTop.poset = poset;
            finalQTopList.push_back(conQTop);
//		}
    }

	return finalQTopList;

}

         // Compares two int pairs for ascending order.
/*     bool compareIntPairs(Intpair i1, Intpair i2)
     {
        if (i1.first < i2.first || (i1.first == i2.first && i1.second < i2.second)) return true;
            else
                return false;
        }*/

void *QuandleTopologies::QTopGenerateParallel(void *threadarg){



    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;


    int maxSize = pow(2, my_data->QOrder) - 1;

	for (int i = my_data->startNum; i <= my_data->endNum; i++)
	{ //for each Hasse Relation
        vector<vector<Intpair>> hasseTabCombo;
//For each QOrder Permutation ((1,2,3), (1,3,2), (2,1,3), etc), prepare the relationships tab.
        for (int j = 0; j < my_data->qOrderCombo.size(); j++){
            Intpair myPair;
            vector<Intpair> hasseTab;
            for (int k = 0; k < my_data->HasseRelTab[i].HasseTab.size(); k++){
                myPair.first = my_data->qOrderCombo[j][my_data->HasseRelTab[i].HasseTab[k].first];
                myPair.second = my_data->qOrderCombo[j][my_data->HasseRelTab[i].HasseTab[k].second];
                hasseTab.push_back(myPair);
            }
            sort(hasseTab.begin(), hasseTab.end(), compareIntPairs);
            bool matchFound = false;
// Check if the relationship tab is duplicate. If not, add to the relationship tables combo list for the current Hasse Relation
            for (int k = 0; k < hasseTabCombo.size(); k++){
                if (hasseTabCombo[k] == hasseTab){
                    matchFound = true;
                    break;
                }
            }
            if (matchFound == false) hasseTabCombo.push_back(hasseTab);
        }



// For each relationship permutation, check for continuity for each of the quandles by:
// finding product of each relationship with each the other in the current permutation and with each trivial
// relationship ( 1 <= 1, 2 <= 2, 3 <= 3, etc). If the product on both LHS and RHS matches one of the relations
// in current permutation, this is good. If any of the products fail, the result is not continuous.
        vector <HasseQuandleContinuous>myHQuanCont;
        for (int j = 0; j < hasseTabCombo.size(); j++){
            vector<Intpair> hasseTab = hasseTabCombo[j];
            vector<Intpair> hasseTabBase = hasseTabCombo[0];
            for (int m = 0; m < my_data->qArray.size(); m++) { //For each quandle
                vector<vector<int>> qTable = my_data->qArray[m];
                bool failed = false;
                for (int k = 0; k < hasseTab.size(); k++){
                    int matchCount = 0;
                    for (int l = k+1; l < hasseTab.size(); l++){
                        Intpair product1, product2;
                        product1.first = qTable[hasseTab[k].first - 1][hasseTab[l].first - 1];
                        product1.second = qTable[hasseTab[k].second - 1][hasseTab[l].second - 1];

                        product2.first = qTable[hasseTab[l].first - 1][hasseTab[k].first - 1];
                        product2.second = qTable[hasseTab[l].second - 1][hasseTab[k].second - 1];
                        matchCount = 0;
                        if (product1.first == product1.second) matchCount += 1;
                        if (product2.first == product2.second) matchCount += 1;
                        for (int n = 0; n < hasseTab.size(); n++){
                            if (product1 == hasseTab[n]) matchCount += 1;
                            if (product2 == hasseTab[n]) matchCount += 1;

                            if (matchCount == 2) break; //>= 1) break;
                        }
                        if (matchCount < 2) { //< 1) {
                            failed = true;
                            break;
                        }
                    }
                    if (k < (hasseTab.size() - 1) && matchCount < 2) { //< 1 ) {
                       failed = true;
                       break;
                    }

// Check product with trivial Hasse relationship
                    for (int l = 0; l < my_data->QOrder; l++){
                        Intpair product1, product2;
//                        product1.first = qTable[hasseTab[k].first - 1][l];
//                        product1.second = qTable[hasseTab[k].second - 1][l];

                        product2.first = qTable[l][hasseTab[k].first - 1];
                        product2.second = qTable[l][hasseTab[k].second - 1];
                        matchCount = 0;
//                        if (product1.first == product1.second) matchCount += 1;
                        if (product2.first == product2.second) matchCount += 1;
                        for (int n = 0; n < hasseTab.size(); n++){
//                            if (product1 == hasseTab[n]) matchCount += 1;
                            if (product2 == hasseTab[n]) matchCount += 1;

                            if (matchCount >= 1) break; //== 2) break; //>= 1) break;
                        }
                        if (matchCount < 1) { //< 2) { //< 1) {
                            failed = true;
                            break;
                        }

                    }
                    if (matchCount < 1) { //< 2) { //< 1) {
                       failed = true;
                       break;
                    }
                }
                if (failed == false){
                    bool matched = false;
                    HasseQuandleContinuous myHasseQCont;
                    myHasseQCont.TopIndex = my_data->HasseRelTab[i].TopIndex;
                    myHasseQCont.HasseTab = hasseTabBase;
                    myHasseQCont.QTabIndex = m;
                    for (int l = 0; l < myHQuanCont.size(); l++){
                        if(myHasseQCont.TopIndex == myHQuanCont[l].TopIndex && myHasseQCont.QTabIndex == myHQuanCont[l].QTabIndex){
                            matched = true;
                            break;
                        }
                    }
                    if (matched == false) myHQuanCont.push_back(myHasseQCont);
                }
            }
        }
        for (int j = 0; j < myHQuanCont.size(); j++){
            my_data->hQuanCont.push_back(myHQuanCont[j]);
        }
	}
        //sort( my_data->hQuanCont.begin(), my_data->hQuanCont.end() );
        //my_data->hQuanCont.erase( unique( my_data->hQuanCont.begin(), my_data->hQuanCont.end() ), my_data->hQuanCont.end() );
		cout << "Thread: " << my_data->thread_id << "  Quan Hasse count: " << my_data->hQuanCont.size() << endl;
	//Add top element to topology
	int test = 0;
	pthread_exit(NULL);




}

