/**
	\author: Trasier
	\date:	 2019/11/18
*/
#include <bits/stdc++.h>
using namespace std;

#include "util.h"
#include "global.h"

void GDP() {
	while (pos < n) {
#ifdef WATCH_MEM
		if (pos % 20000 == 0) {
			watchSolutionOnce(getpid(), usedMemory);
		}
#endif  
		if (pos % 2000 == 0) {
			t3 = clock();
			if (t3-t2 >= clockLimit) {
				dumpResult("GDP");			
				return ;
			}
		}
		
		for (int i = 0; i < m; ++ i) {
			updateDriver(i, R[pos].tim);
		}

        tsd = clock();
        vector<int> car;
        // in case there s and e are not linked together, it really did.
        if (R[pos].len < INF_WEIGHT) {
            car = single_search(R[pos].s, R[pos].ddl-R[pos].tim);
		}

		////assign the request to some taxi
		assignTaxi(car);
	}
	
 	for (int i = 0; i < m; ++ i) {
		finishTaxi(i);
	}
	t3 = clock();
	dumpResult("GDP");
}

int main(int argc, char **args) {
#ifdef LOCAL_DEBUG
	vertexFile = string("./road/chengdu.node");
	edgeFile = string("./road/chengdu.edge");
	pathLabelFile = string("./road/chengdu.label");
	orderFile = string("./road/chengdu.order");
	dataFile = string("chengdu_data.dat");
	requestFile = string("chengdu_order.dat");
	desFile = string("res.txt");
#endif

    if (argc > 4) vertexFile = string(args[1]);
    if (argc > 4) edgeFile = string(args[2]);
	if (argc > 4) pathLabelFile = string(args[3]);
	if (argc > 4) orderFile = string(args[4]);
	if (argc > 5) dataFile = string(args[5]);
	if (argc > 6) requestFile = string(args[6]);
	if (argc > 7) {
		desFile = string(args[7]);
		freopen(desFile.c_str(), "w", stdout);
	}

	readInput();
	
	t1 = clock();
	initGrid();
	t2 = clock();

	GDP();
	freeMemory();
	fflush(stdout);
	
    return 0;
}
