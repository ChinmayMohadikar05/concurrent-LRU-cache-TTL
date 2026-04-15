#pragma once

#include <mutex>
#include <unordered_map>
#include <queue>
#include <chrono>
#include <thread>
//#include <shared_mutex>

using namespace std;

class LRUCacheTTL {
private:
    struct Node {
        int key, value;
        Node* prev;
        Node* next;
        Node(int k, int v);
    };

    int capacity;

    unordered_map<int, Node*> cache;
	// track latest expiry for stale heap validation
    unordered_map<int, chrono::steady_clock::time_point> latestExpiry;

    using pii = pair<chrono::steady_clock::time_point, int>;
	// use minHeap to remove expired keys
    priority_queue<pii, vector<pii>, greater<pii>> minHeap;

    Node* head;
    Node* tail;

	// shared_mutex is used in modern compilers (GCC 7+).
	// Fallback to mutex is provided for compatibility. 
    mutable mutex mtx;
    bool stopCleaner;
    thread cleanerThread;

    // DLL helper functions
    void addNode(Node* node);
    void deleteNode(Node* node);
    void moveToFront(Node* node);
    void removeLRU();
    void removeKey(int key);
    bool isExpired(int key);
    void cleanerTask();

public:
    LRUCacheTTL(int cap);
    ~LRUCacheTTL();

    int get(int key);
    void put(int key, int value, int ttlSeconds);
};
