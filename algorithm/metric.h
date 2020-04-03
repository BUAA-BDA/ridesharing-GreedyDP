#ifndef METRIC_H
#define METRIC_H

#include<bits/stdc++.h>
using namespace std;
#include "labels.h"
#include "global.h"
#include "lrucache.h"

typedef unordered_map<long long, EdgeID> HashMap;

class ShortestPath {
private:
	PLabel pathLabel;
	double distance(vertex *a, vertex *b);
	long long getKey(NodeID start, NodeID end) {
		if (start > end) swap(start, end);
		return 1LL * start * numOfVertices + end;
	}

public:
	double sdqn;
	double spqn;
	vector<NodeID> rank;
	vector<NodeID> inv;
	vector<vertex> vertices;
	vector<edge> edges;
	HashMap edgeHash;
	cache_t<long long,double> distCache;
	cache_t<long long,pair<double,vector<NodeID> > > npathCache;
	cache_t<long long,pair<double,vector<EdgeID> > > epathCache;
	
	ShortestPath(string vertexFile, string edgeFile, string pathLabelFile, string orderFileName);
	double shortestDistance(NodeID start, NodeID end);
	double shortestPathByNode(NodeID start, NodeID end, vector<NodeID>& Q);
	double shortestPathByEdge(NodeID start, NodeID end, vector<EdgeID>& Q);
	double shortestPathByWeight(NodeID start, NodeID end, vector<NodeID>& Q, vector<EdgeWeight>& wQ);
	double pathDistanceByNode(NodeID start, const vector<NodeID>& path);
	double pathDistanceByEdge(NodeID start, const vector<EdgeID>& path);
};

#endif
