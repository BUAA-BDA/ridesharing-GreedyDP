/**
	\author:	Trasier
	\date:		2019.6.18
*/
#ifndef GLOBAL_H
#define GLOBAL_H
#include <bits/stdc++.h>
using namespace std;

typedef int NodeID;
typedef int EdgeID;
typedef double EdgeWeight;

struct vertex {
	NodeID id;
	double x;
	double y;
};

struct edge {
	NodeID u, v;
	EdgeWeight w;
};

extern int numOfVertices;
extern int numOfEdges;
extern const int INF_WEIGHT;
extern const double EPS;
extern const double INF;
extern const double speed;
extern const double MAX_speed;
extern const int CACHE_SIZE;
extern int usedMemory;
extern double usedTime;
extern const double clockLimit;

int dcmp(double x);
double eucDist(const vertex& a, const vertex& b);

#endif
