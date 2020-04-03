/**
	\Author: Trasier
	\Date:   2019.11.18
*/

#include "metric.h"

double ShortestPath::distance(vertex *a, vertex *b) {
	double d1 = a->x - b->x;
	double d2 = a->y - b->y;
	return sqrt(d1 * d1 + d2 * d2);
}

ShortestPath::ShortestPath(string vertexFileName, string edgeFileName, string pathLabelFileName, string orderFileName) {
    // read vertices
    ifstream vertexFile;
    vertexFile.open(vertexFileName.c_str());
	if (!vertexFile.is_open()) {
		printf("%s does not exist\n", vertexFileName.c_str());
		exit(0);
	}
	cout << "begin coordinates" << endl;
    vertexFile >> numOfVertices;
	vertices.resize(numOfVertices);
    for(int i=0; i<numOfVertices; ++i) {
        vertexFile >> vertices[i].x >> vertices[i].y;
		vertices[i].id = i;
    }
    vertexFile.close();	
	cout << "end coordinates" << endl;
	
	// read edges
    ifstream edgeFile;
    edgeFile.open(edgeFileName.c_str());
	if (!edgeFile.is_open()) {
		printf("%s does not exist\n", edgeFileName.c_str());
		exit(0);
	}	
	cout << "begin edges" << endl;
    edgeFile >> numOfEdges;
	edges.resize(numOfEdges);
	edgeHash.clear();
    for(int i=0; i<numOfEdges; ++i) {
        edgeFile >> edges[i].u >> edges[i].v >> edges[i].w;
		edgeHash[1LL*edges[i].u*numOfVertices+edges[i].v] = i;
    }
    edgeFile.close();
	cout << "end edges" << endl;
	
	// read labels
	cout << "begin labels" << endl;
	pathLabel.load_labels(pathLabelFileName.c_str());
	cout << "end labels" << endl;
	
	// read rank
	ifstream orderFile;
	orderFile.open(orderFileName.c_str());	
	if (!orderFile.is_open()) {
		printf("%s does not exist\n", orderFileName.c_str());
		exit(0);
	}	
	
	NodeID tv;
	rank.resize(numOfVertices);
	inv.resize(numOfVertices);
	cout << "begin orders" << endl;
	for(int i = 0; i < numOfVertices; ++i){
		orderFile >> tv;
		rank[tv] = i;
		inv[i] = tv;
	}
	orderFile.close();
	cout << "end orders" << endl;
	
	distCache.resize(CACHE_SIZE);
	npathCache.resize(CACHE_SIZE);
	epathCache.resize(CACHE_SIZE);	
}

double ShortestPath::shortestDistance(NodeID start, NodeID end) {
	sdqn += 1.0;
	long long key = getKey(start, end);
	double ret = 0.0;
	
	if (distCache.query(key, ret) == false) {
		if (start == end) {
			ret = 0.0;
		} else {
			ret = pathLabel.query_p(start, end);
		}
		distCache.insert(key, ret);
	}
	
	return ret;
}

double ShortestPath::shortestPathByNode(NodeID start, NodeID end, vector<NodeID>& npath) {
	spqn += 1.0;
	npath.clear();
	pair<double,vector<NodeID> > ret = make_pair(0.0, vector<NodeID>());
	long long key = getKey(start, end);
	double distance;
	
	if (npathCache.query(key, ret) == false) {
		if (start == end) {
			distance = 0.0;
			npath.push_back(start);
			npath.push_back(end);
		} else {
			distance = pathLabel.query_path(start, end, rank, inv, npath);
		}
		ret.first = distance;
		ret.second = npath;
		npathCache.insert(key, ret);
	} else {
		distance = ret.first;
		npath = ret.second;
	}
	
	return distance;
}

double ShortestPath::shortestPathByEdge(NodeID start, NodeID end, vector<EdgeID>& epath) {
	spqn += 1.0;
	epath.clear();
	pair<double,vector<EdgeID> > ret = make_pair(0.0, vector<EdgeID>());
	long long key = getKey(start, end);
	double distance;
	
	if (epathCache.query(key, ret) == false) {
		if (start == end) {
			distance = 0.0;
			epath.push_back(-(start+1));
		} else {
			vector<NodeID> vtmp;
			distance = pathLabel.query_path(start, end, rank, inv, vtmp);
			for (int i=1; i<vtmp.size(); ++i) {
				EdgeID eid = edgeHash[1LL*vtmp[i-1]*numOfVertices+vtmp[i]];
				epath.push_back(eid);
			}
		}
		ret.first = distance;
		ret.second = epath;
		epathCache.insert(key, ret);
	} else {
		distance = ret.first;
		epath = ret.second;
	}
	
	return distance;
}

double ShortestPath::shortestPathByWeight(NodeID start, NodeID end, vector<NodeID>& Q, vector<EdgeWeight>& wQ) {
	Q.clear();
	wQ.clear();
	if (start == end) {
		Q.push_back(end);
		wQ.push_back(0);
		return 0.0;
	}
	
	vector<NodeID> vtmp;
	double distance = shortestPathByNode(start, end, vtmp);
	
	for (int i=1; i<vtmp.size(); ++i) {
		EdgeID eid = edgeHash[1LL*vtmp[i-1]*numOfVertices+vtmp[i]];
		Q.push_back(edges[eid].v);
		wQ.push_back(edges[eid].w);
	}
	
	return distance;	
	
}

double ShortestPath::pathDistanceByNode(NodeID start, const vector<NodeID>& path) {
	if(path.size() == 0) return 0;
	double ret = 0.0;
	NodeID tmp = start;
	
	for (auto iter=path.begin(); iter!=path.end(); ++iter) {
		ret += shortestDistance(tmp, *iter);
		tmp = *iter;
	}
	
	return ret;
}

double ShortestPath::pathDistanceByEdge(NodeID start, const vector<EdgeID>& path) {
	if(path.size() == 0) return 0;
	double ret = 0.0;
	
	if (*path.begin() < 0) {
		NodeID idx = -(*path.begin())-1;
		ret += shortestDistance(start, idx);
	} else {
		NodeID idx = edges[*path.begin()].u;
		ret += shortestDistance(start, idx);
	}
	for (auto iter=path.begin(); iter!=path.end(); ++iter) {
		if (*iter >= 0)
			ret += edges[*iter].w;
	}
	
	return ret;
}