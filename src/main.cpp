#include "LRU_cache.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    LRUCacheTTL cache(2);

    cache.put(11, 45, 10);
    cache.put(12, 46, 20);

    cout << cache.get(11) << endl;

    std::this_thread::sleep_for(chrono::seconds(12));

    cout << cache.get(11) << endl; // returns -1 as it is expired

    cache.put(13, 47, 5);

    cout << cache.get(12) << endl;

    return 0;
}