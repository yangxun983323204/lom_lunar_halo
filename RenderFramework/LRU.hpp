#pragma once
#include <unordered_map>
#include <stdint.h>

using std::unordered_map;

template <typename T1, typename T2>
class CacheNode {
public:
	T1 key;
	T2 value;
	CacheNode* pre, * next;
	CacheNode(T1 k, T2 v) : key(k), value(v), pre(nullptr), next(nullptr) {}
};

template <typename T1, typename T2>
class LRUCache {
private:
	int size;
	CacheNode<T1, T2>* head, * tail;
	unordered_map<T1, CacheNode<T1, T2>*> mp;

public:
	LRUCache(int capacity) : size(capacity), head(nullptr), tail(nullptr) {}

	uint32_t getSize()
	{
		return mp.size();
	}

	void remove(CacheNode<T1, T2>* node) {
		if (nullptr != node->pre) {
			node->pre->next = node->next;
		}
		else {
			head = node->next;
		}
		if (nullptr != node->next) {
			node->next->pre = node->pre;
		}
		else {
			tail = node->pre;
		}
	}

	void setHead(CacheNode<T1, T2>* node) {
		node->next = head;
		node->pre = nullptr;
		if (nullptr != head) {
			head->pre = node;
		}
		head = node;
		if (nullptr == tail) {
			tail = head;
		}
	}

	void set(T1 key, T2 value) {
		auto it = mp.find(key);
		if (it != mp.end()) {
			CacheNode<T1, T2>* node = it->second;
			node->value = value;
			remove(node);
			setHead(node);
		}
		else {
			CacheNode<T1, T2>* node = new CacheNode<T1, T2>(key, value);
			if (mp.size() >= size) {
				auto it = mp.find(tail->key);
				remove(tail);
				mp.erase(it);
			}
			setHead(node);
			mp[key] = node;
		}
	}

	void get(T1 key, T2& value) {

		auto it = mp.find(key);
		if (it != mp.end()) {
			CacheNode<T1, T2>* node = it->second;
			remove(node);
			setHead(node);
			value = node->value;
		}
	}

	T2&& removeLast()
	{
		CacheNode<T1, T2>* last = tail;
		auto it = mp.find(tail->key);
		remove(tail);
		mp.erase(it);
		auto t2 = last->value;
		delete last;
		return std::move(t2);
	}
};