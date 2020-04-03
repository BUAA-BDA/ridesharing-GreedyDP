/**
	\author: Trasier
	\date: 2019/12/10
*/
#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <bits/stdc++.h>
using namespace std;

template<typename key_t, typename value_t>
class cache_t {
public:	
	typedef pair<key_t, value_t> kv_pair_t;
	typedef typename list<kv_pair_t>::iterator list_iter_t;
	typedef typename unordered_map<key_t, list_iter_t>::iterator index_iter_t;

private:
	list<kv_pair_t> values;
	unordered_map<key_t, list_iter_t> index;
	int max_size;
	int sz;
	long long miss, tot;
	
public:
	cache_t(int _max_size=1024) : max_size(_max_size) {
		values.resize(max_size);
		sz = 0;
		miss = tot = 0;
	}
	
	bool empty() {
		return sz == 0;
	}
	
	int size() {
		return sz;
	}
	
	void resize(int _max_size) {
		max_size = _max_size;
		values.resize(max_size);
		index.clear();
		sz = 0;
		miss = tot = 0;		
	}
	
	void clear() {
		sz = 0;
		miss = tot = 0;
		index.clear();
		values.clear();
	}
	
	void insert(key_t key,value_t value);
	bool query(key_t key, value_t& value);
	bool find(key_t key);
};


template<typename key_t, typename value_t>
void cache_t<key_t,value_t>::insert(key_t key, value_t value) {
	index_iter_t iter = index.find(key);
	
	++tot;
	if (iter == index.end()) {
		if (sz == max_size) {
			--sz;
			index.erase(values.rbegin()->first);
			values.pop_back();
		}
		values.push_front(kv_pair_t(key, value));
		index[key] = values.begin();
		++sz;
		++miss;
	} else {
		values.splice(values.begin(), values, iter->second);	
	}
}

template<typename key_t, typename value_t>
bool cache_t<key_t,value_t>::query(key_t key, value_t& value) {
	index_iter_t iter = index.find(key);
	
	++tot;
	if (iter == index.end()) {
		++miss;
		return false;
	}
	
	value = iter->second->second;
	values.splice(values.begin(), values, iter->second);
	
	return true;
}

template<typename key_t, typename value_t>
bool cache_t<key_t,value_t>::find(key_t key) {
	index_iter_t iter = index.find(key);
	
	++tot;
	if (iter == index.end()) {
		++miss;
		return false;
	}
	
	values.splice(values.begin(), values, iter->second);
	
	return true;
}

#endif
