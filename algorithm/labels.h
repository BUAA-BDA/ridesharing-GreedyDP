/*
An Experimental Study on Hub Labeling based Shortest Path Algorithms [Experiments and Analyses]

Authors: Ye Li, Leong Hou U, Man Lung Yiu, Ngai Meng Kou
Contact: yb47438@umac.mo
Affiliation: University of Macau

The MIT License (MIT)

Copyright (c) 2016 University of Macau

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#ifndef LABELS_H
#define LABELS_H

#include <limits>
#include <climits>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <malloc.h>
#include <xmmintrin.h>
#include <omp.h>
#include <bitset>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

#include "global.h"

struct index_t_p {
	NodeID* spt_v;
	EdgeWeight* spt_d;
}__attribute__((aligned(64)));  // Aligned for cache lines;

struct index_t_path {
	vector<NodeID> spt_v;
	vector<NodeID> spt_p;//parent nodes
	vector<EdgeWeight> spt_d;

	NodeID size() {
		return spt_v.size();
	}
};

struct index_t_path_p {
	NodeID* spt_v;
	NodeID* spt_p;
	EdgeWeight* spt_d;
};

struct token_t {
	NodeID* sptc_v; // sptc_v[0] is the root
	EdgeWeight* sptc_d;	 // |*| = k + 1, sptc_d[0] is the number of children - k
	unsigned char* sptc_fbv; // first-level bit vector
	unsigned char* sptc_sbv; // second-level bit vector
	NodeID* sptc_pathv; // intermediate point for a path
}__attribute__((aligned(64)));

class CLabel {

public:
	token_t* supertokenindex_p;
	token_t* tokenindex_p;
	NodeID* anchor_p;
	NodeID numOfTokens;
	long total_children;
	
	
	token_t* r_supertokenindex_p;
	token_t* r_tokenindex_p;
	NodeID* r_anchor_p;
	NodeID r_numOfTokens;
	long r_total_children;
	
	
	void load_labels(const char* load_filename) {
		
		total_children = 0;
		
		tokenindex_p = NULL;
		anchor_p = NULL;

		ifstream ifs(load_filename);
		NodeID isize = 0;
		ifs.read((char*)&isize, sizeof(isize));
		numOfVertices = isize;

		anchor_p = (NodeID*)memalign(64, numOfVertices * sizeof(NodeID));
		NodeID anchor_id;
		for (NodeID v = 0; v < numOfVertices; ++v) {
				ifs.read((char*)&anchor_id, sizeof(anchor_id));
				anchor_p[v] = anchor_id;
		}
		
		ifs.read((char*)&isize, sizeof(isize));
		numOfTokens = isize;		
		tokenindex_p = (token_t*)memalign(64, numOfTokens * sizeof(token_t));
		
		EdgeWeight csize;
		NodeID cid;
		EdgeWeight cd;
		for (NodeID v = 0; v < numOfTokens; ++v) {
			token_t& tt = tokenindex_p[v];
		
			ifs.read((char*)&cid, sizeof(cid));
			ifs.read((char*)&csize, sizeof(csize));
			
			tt.sptc_v = (NodeID*)memalign(64, (csize + 1) * sizeof(NodeID));
			tt.sptc_d = (EdgeWeight*)memalign(64, (csize + 1 ) * sizeof(EdgeWeight));
			
			total_children += (csize + 1);
			
			tt.sptc_v[0] = cid;
			tt.sptc_d[0] = csize;
			
			for (NodeID i = 0; i < csize; ++i) {
				ifs.read((char*)&cid, sizeof(cid));
				ifs.read((char*)&cd, sizeof(cd));
				tt.sptc_v[i + 1] = cid;
				tt.sptc_d[i + 1] = cd;		
			}
		}
		ifs.close();
	}
	

	EdgeWeight query_p(NodeID s, NodeID t, long ts, vector<NodeID>& dis_vec, vector<long>& ts_vec, vector<NodeID>& que, vector<EdgeWeight>& que_d) {
		if(s==t) return 0;
		
		EdgeWeight distance = INF_WEIGHT;
		
		NodeID anchor_s = anchor_p[s];
		NodeID anchor_t = anchor_p[t];

		NodeID que_t0 = 0, que_t1 = 0, que_h = 0;
		
		que_d[que_h] = 0;
		que[que_h++] = anchor_s;
		que_t1 = que_h;
		
		if(anchor_s < numOfVertices){
			if(ts_vec[anchor_s] != ts){
				ts_vec[anchor_s] = ts;
				dis_vec[anchor_s] = 0;
			}			
		}
		else{
			for (; que_t0 < que_h;) {
				for (NodeID que_i = que_t0; que_i < que_t1; ++que_i) {
					NodeID tid = que[que_i];
					EdgeWeight tdis = que_d[que_i];
					
					const token_t& token_v = tokenindex_p[tid - numOfVertices];
					
					_mm_prefetch(&token_v.sptc_v[0], _MM_HINT_T0);
					_mm_prefetch(&token_v.sptc_d[0], _MM_HINT_T0);
					
					NodeID r = token_v.sptc_v[0];
					EdgeWeight csize = token_v.sptc_d[0];
					
					// hashing, can be replaced by 1024 linear probing for efficiency.
					if(ts_vec[r] != ts){
						ts_vec[r] = ts;
						dis_vec[r] = tdis;
					}
					
					for (NodeID i = 0; i < csize; ++i){
						NodeID w = token_v.sptc_v[i+1];
						EdgeWeight w_d = token_v.sptc_d[i+1] + tdis;
						if( w < numOfVertices){// hashing, can be replaced by 1024 linear probing for efficiency.
							if(ts_vec[w] != ts){
								ts_vec[w] = ts;
								dis_vec[w] = w_d;
							}
						}else{
							que_d[que_h] = w_d;
							que[que_h++] = w;
						}
					}
				}
				que_t0 = que_t1;
				que_t1 = que_h;
			}
		}
		
		
		que_t0 = 0, que_t1 = 0, que_h = 0;
		que_d[que_h] = 0;
		que[que_h++] = anchor_t;
		
		if(anchor_t < numOfVertices){
			if(ts_vec[anchor_t] == ts){
				EdgeWeight current_dis = dis_vec[anchor_t] + 0;
				if(current_dis < distance)
					distance = current_dis;
			}
		}else{
			que_t1 = que_h;
			for (; que_t0 < que_h;) {
				for (NodeID que_i = que_t0; que_i < que_t1; ++que_i) {
					NodeID tid = que[que_i];
					EdgeWeight tdis = que_d[que_i];
					
					const token_t& token_v = tokenindex_p[tid - numOfVertices];
					
					_mm_prefetch(&token_v.sptc_v[0], _MM_HINT_T0);
					_mm_prefetch(&token_v.sptc_d[0], _MM_HINT_T0);
					
					NodeID r = token_v.sptc_v[0];
					EdgeWeight csize = token_v.sptc_d[0];
					
					// hashing, can be replaced by 1024 linear probing for efficiency.
					if(ts_vec[r] == ts){
						EdgeWeight current_dis = dis_vec[r] + tdis;
						if(current_dis < distance)
							distance = current_dis;
					}
					
					for (NodeID i = 0; i < csize; ++i){
						NodeID w = token_v.sptc_v[i+1];
						EdgeWeight w_d = token_v.sptc_d[i+1] + tdis;
						if( w < numOfVertices){
						// hashing, can be replaced by 1024 linear probing for efficiency.
							if(ts_vec[w] == ts){
								EdgeWeight current_dis = dis_vec[w] + w_d;
								if(current_dis < distance)
									distance = current_dis;
							}
						}else{							
							que_d[que_h] = w_d;
							que[que_h++] = w;
						}
					}
				}
				que_t0 = que_t1;
				que_t1 = que_h;
			}
		}		
		return distance;
	}
	
	
};


class Label {

public:	
	index_t_p* index_p;

	Label() {
	}

	~Label() {
	}

	EdgeWeight query_p(NodeID s, NodeID t) {
		if (s == t) return 0;
		
		EdgeWeight distance = INF_WEIGHT;

		const index_t_p &idx_s = index_p[s];
		const index_t_p &idx_t = index_p[t];

		_mm_prefetch(&idx_s.spt_v[0], _MM_HINT_T0);
		_mm_prefetch(&idx_t.spt_v[0], _MM_HINT_T0);
		_mm_prefetch(&idx_s.spt_d[0], _MM_HINT_T0);
		_mm_prefetch(&idx_t.spt_d[0], _MM_HINT_T0);

		for (int i = 0, j = 0; ; ) {
			NodeID v1 = idx_s.spt_v[i], v2 = idx_t.spt_v[j];

			if (v1 == numOfVertices) break;  // Sentinel

			if (v1 == v2) {
				EdgeWeight td = idx_s.spt_d[i] + idx_t.spt_d[j];
				if (td < distance) distance = td;
				++i;
				++j;
			} 
			else {
				i += v1 < v2 ? 1 : 0;
				j += v1 > v2 ? 1 : 0;
			}
		}
		return distance;
	}
	

	void load_labels(const char* load_filename) {
		index_p = NULL;

		ifstream ifs(load_filename);
		if (!ifs.is_open()) {
			printf("%s does not exist\n", load_filename);
			exit(0);
		}	
	
		NodeID isize = 0;
		ifs.read((char*)&isize, sizeof(isize));
		numOfVertices = isize;

		index_p = (index_t_p*)memalign(64, numOfVertices * sizeof(index_t_p));
		


		for (NodeID v = 0; v < numOfVertices; ++v) {
			index_t_p &idx = index_p[v];
			ifs.read((char*)&isize, sizeof(isize));

			idx.spt_v = (NodeID*)memalign(64, isize * sizeof(NodeID));
			idx.spt_d = (EdgeWeight*)memalign(64, isize * sizeof(EdgeWeight));

			for (NodeID i = 0; i < isize; ++i) {
				NodeID hub;
				EdgeWeight hub_weight;
				ifs.read((char*)&hub, sizeof(hub));
				ifs.read((char*)&hub_weight, sizeof(hub_weight));

				idx.spt_v[i] = hub;
				idx.spt_d[i] = hub_weight;

			}
		}
		ifs.close();
	}


};

class PLabel {

public:
	vector<index_t_path> index_;
	index_t_path_p* index_p;

	PLabel() {
		index_.resize(numOfVertices);
	}

	~PLabel() {
		Free();
	}

	EdgeWeight query_p(NodeID s, NodeID t) {
		if (s == t) return 0.0;
		
		EdgeWeight distance = INF_WEIGHT;

		const index_t_path_p &idx_s = index_p[s];
		const index_t_path_p &idx_t = index_p[t];

		_mm_prefetch(&idx_s.spt_v[0], _MM_HINT_T0);
		_mm_prefetch(&idx_t.spt_v[0], _MM_HINT_T0);
		_mm_prefetch(&idx_s.spt_d[0], _MM_HINT_T0);
		_mm_prefetch(&idx_t.spt_d[0], _MM_HINT_T0);

		for (int i = 0, j = 0; ; ) {
			NodeID v1 = idx_s.spt_v[i], v2 = idx_t.spt_v[j];

			if (v1 == numOfVertices) break;  // Sentinel

			if (v1 == v2) {
				EdgeWeight td = idx_s.spt_d[i] + idx_t.spt_d[j];
				if (td < distance) {
					distance = td;
				}
				++i;
				++j;
			}
			else {
				i += v1 < v2 ? 1 : 0;
				j += v1 > v2 ? 1 : 0;
			}
		}

		return distance;
	}

	EdgeWeight query_path(NodeID s, NodeID t, vector<NodeID>& rank, vector<NodeID>& inv, vector<NodeID>& ret) {
		ret.clear();
		if (s == t) return 0.0;
		
		EdgeWeight distance = INF_WEIGHT;
		NodeID meetnode = numOfVertices;
		NodeID s_parent;
		NodeID t_parent;

		const index_t_path_p &idx_s = index_p[s];
		const index_t_path_p &idx_t = index_p[t];

		_mm_prefetch(&idx_s.spt_v[0], _MM_HINT_T0);
		_mm_prefetch(&idx_t.spt_v[0], _MM_HINT_T0);
		_mm_prefetch(&idx_s.spt_d[0], _MM_HINT_T0);
		_mm_prefetch(&idx_t.spt_d[0], _MM_HINT_T0);
		_mm_prefetch(&idx_s.spt_p[0], _MM_HINT_T0);
		_mm_prefetch(&idx_t.spt_p[0], _MM_HINT_T0);

		for (int i = 0, j = 0; ; ) {
			NodeID v1 = idx_s.spt_v[i], v2 = idx_t.spt_v[j];

			if (v1 == numOfVertices) break;  // Sentinel

			if (v1 == v2) {
				EdgeWeight td = idx_s.spt_d[i] + idx_t.spt_d[j];
				if (td < distance) {
					distance = td;
					meetnode = v1;
					s_parent = idx_s.spt_p[i];
					t_parent = idx_t.spt_p[j];
				}
				++i;
				++j;
			}
			else {
				i += v1 < v2 ? 1 : 0;
				j += v1 > v2 ? 1 : 0;
			}
		}

		//Next, retrieve path from s - meetnode and meetnode - t.
		vector<NodeID> path_from_s;
		vector<NodeID> path_to_t;
		path_from_s.push_back(s_parent);
		path_to_t.push_back(t_parent);
		
		int operation = 0;

		NodeID inv_meetnode = inv[meetnode];

		while (path_from_s.back() != inv_meetnode) {
			const index_t_path_p &idx_from_s = index_p[path_from_s.back()];

			_mm_prefetch(&idx_from_s.spt_v[0], _MM_HINT_T0);
			_mm_prefetch(&idx_from_s.spt_p[0], _MM_HINT_T0);
			
			for (int i = 0; ; ++i) {
				operation++;
				if (idx_from_s.spt_v[i] == numOfVertices) break;
				if (idx_from_s.spt_v[i] == meetnode) {
					path_from_s.push_back(idx_from_s.spt_p[i]);
					break;
				}
			}
		}

		while (path_to_t.back() != inv_meetnode) {
			const index_t_path_p &idx_to_t = index_p[path_to_t.back()];
			
			_mm_prefetch(&idx_to_t.spt_v[0], _MM_HINT_T0);
			_mm_prefetch(&idx_to_t.spt_p[0], _MM_HINT_T0);
			for (int i = 0; ; ++i) {
				operation++;
				if (idx_to_t.spt_v[i] == numOfVertices) break;
				if (idx_to_t.spt_v[i] == meetnode) {
					path_to_t.push_back(idx_to_t.spt_p[i]);
					break;
				}
			}
		}
		
		ret.push_back(s);
		for (int i=0; i<path_from_s.size()-1; ++i) {
			ret.push_back(path_from_s[i]);
		}
		for (int i=path_to_t.size()-1; i>=0; --i) {
			ret.push_back(path_to_t[i]);
		}
		ret.push_back(t);
		
		// EdgeWeight alldis = 0;
		// cout << distance << "," << query_p(s, inv_meetnode)+query_p(t, inv_meetnode) << endl;
			
		// for (int i = 0; i < path_from_s.size() - 1; ++i) {
			// alldis += query_p(path_from_s[i], path_from_s[i + 1]);
			// cout << "s: " << path_from_s[i] << "," << path_from_s[i + 1] << endl;
		// }
		// for (int i = 0; i < path_to_t.size() - 1; ++i) {
			// alldis += query_p(path_to_t[i], path_to_t[i + 1]);
			// cout <<"t: " <<  path_to_t[i] << "," << path_to_t[i + 1] << endl;
		// }
		// if (distance != alldis)
			// cout << "a?" << endl;
		// cout << distance << "," << alldis << "," << path_from_s.size() + path_to_t.size() << endl;
		// cout << s << "," << t << "," << inv_meetnode << "   " << distance << "vs." << alldis << endl;
		// fflush(stdout);
		
		return distance;
	}
	
	void Free() {
		if (index_.size() == 0) return;
		for (int v = 0; v < numOfVertices; ++v) {
			index_[v].spt_v.clear();
			index_[v].spt_d.clear();
		}
		index_.clear();
	}

	void load_labels(const char* load_filename) {
		index_p = NULL;

		ifstream ifs(load_filename);
		NodeID isize = 0;
		ifs.read((char*)&isize, sizeof(isize));
		numOfVertices = isize;

		index_p = (index_t_path_p*)memalign(64, numOfVertices * sizeof(index_t_path_p));

		for (NodeID v = 0; v < numOfVertices; ++v) {
			index_t_path_p &idx = index_p[v];
			ifs.read((char*)&isize, sizeof(isize));

			idx.spt_v = (NodeID*)memalign(64, isize * sizeof(NodeID));
			idx.spt_p = (NodeID*)memalign(64, isize * sizeof(NodeID));
			idx.spt_d = (EdgeWeight*)memalign(64, isize * sizeof(EdgeWeight));

			for (NodeID i = 0; i < isize; ++i) {
				NodeID hub;
				NodeID hub_parent;
				EdgeWeight hub_weight;
				ifs.read((char*)&hub, sizeof(hub));
				ifs.read((char*)&hub_parent, sizeof(hub_parent));
				ifs.read((char*)&hub_weight, sizeof(hub_weight));
				idx.spt_v[i] = hub;
				idx.spt_p[i] = hub_parent;
				idx.spt_d[i] = hub_weight;
			}
		}
		ifs.close();
	}
};

#endif
