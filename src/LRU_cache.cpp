#include "LRU_cache.h"
#include <iostream>
#include <thread>
#include <chrono>

// Node constructor
LRUCacheTTL::Node::Node(int k, int v) : key(k), value(v), prev(nullptr), next(nullptr) {}

// LRU constructor
LRUCacheTTL::LRUCacheTTL(int cap) : capacity(cap), stopCleaner(false) {
    head = new Node(-1, -1);
    tail = new Node(-1, -1);

    head->next = tail;
    tail->prev = head;

    cleanerThread = thread(&LRUCacheTTL::cleanerTask, this);
}

// LRU destructor
LRUCacheTTL::~LRUCacheTTL() {
	stopCleaner = true;
	if (cleanerThread.joinable()) cleanerThread.join();

	Node* curr = head;
	while (curr) {
		Node* next = curr->next;
		delete curr;
		curr = next;
	}
}

// *************** DLL Helper functions - start ***************
void LRUCacheTTL::addNode(Node* node) {
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

void LRUCacheTTL::deleteNode(Node* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void LRUCacheTTL::moveToFront(Node* node) {
    deleteNode(node);
    addNode(node);
}

void LRUCacheTTL::removeLRU() {
    Node* lru = tail->prev;
    if (lru == head) return;

    int key = lru->key;

    cache.erase(key);
    latestExpiry.erase(key);

    deleteNode(lru);
    delete lru;
}

void LRUCacheTTL::removeKey(int key) {
    auto it = cache.find(key);
    if (it == cache.end()) return;

    Node* node = it->second;

    deleteNode(node);
    delete node;

    cache.erase(it);
    latestExpiry.erase(key);
}

bool LRUCacheTTL::isExpired(int key) {
    auto it = latestExpiry.find(key);
    if (it == latestExpiry.end()) return false;

    return chrono::steady_clock::now() > it->second;
}
// *************** DLL Helper functions - end ***************

// use cleaner thread for active cleanup of expired keys
void LRUCacheTTL::cleanerTask() {
    while (!stopCleaner) {
        this_thread::sleep_for(chrono::milliseconds(500));

        unique_lock<mutex> lock(mtx);
        auto now = chrono::steady_clock::now();

        while (!minHeap.empty()) {
			auto top = minHeap.top();
			auto expiryTime = top.first;
			auto key = top.second;

            if (expiryTime > now) break;

            minHeap.pop();

			// check to Skip stale heap entry
            if (latestExpiry.find(key) == latestExpiry.end() ||
                latestExpiry[key] != expiryTime) {
                continue;
            }

            removeKey(key);
        }
    }
}

// Get API
int LRUCacheTTL::get(int key) {
	if (capacity == 0) return -1;

	// use shared lock to allow multiple threads to read from cache 
	unique_lock<mutex> lock(mtx);

	auto it = cache.find(key);
	if (it == cache.end()) return -1;

	if (isExpired(key)) {
		lock.unlock();
		unique_lock writeLock(mtx);
		removeKey(key);
		return -1;
	}

	Node* node = it->second;

	// change to write lock to modify cache
	// shared mutex is not supported by compiler so following two lines are commented
	//lock.unlock();
	//unique_lock<shared_mutex> writeLock(mtx);

	// race safety - as a precaution re-check existence
	if (cache.find(key) == cache.end()) return -1;

	moveToFront(node);
	return node->value;
}

// Put API
void LRUCacheTTL::put(int key, int value, int ttlSeconds) {
	if (capacity == 0 || ttlSeconds <= 0) return;
	
	// use unique_lock to allow a single thread to modify cache
	unique_lock<mutex> lock(mtx);

	if (cache.find(key) != cache.end()) {
		Node* node = cache[key];
		node->value = value;
		moveToFront(node);
	} else {
		if (cache.size() == capacity) {
			removeLRU();
		}

		Node* node = new Node(key, value);
		cache[key] = node;
		addNode(node);
	}

	auto expiryTime = chrono::steady_clock::now() + chrono::seconds(ttlSeconds);

	latestExpiry[key] = expiryTime;
	minHeap.push({expiryTime, key});
}