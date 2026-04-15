## LRU Cache with TTL (C++)

## Overview
A high-performance in-memory cache system implementing:
- LRU (Least Recently Used) eviction
- TTL (Time-To-Live) expiration
- Concurrent access handling
- Background cleaner thread

## ⚙️ Features
- O(1) get and put operations
- TTL-based automatic expiration
- Min-heap for efficient expiry tracking
- Thread-safe using mutex
- Background cleanup thread

## System Design

### Components:
- Doubly Linked List → Maintains LRU order
- Hash Map → O(1) access
- Min Heap → Efficient TTL expiry
- Background Thread → Active cleanup

## Complexity
| Operation | Time Complexity |
|----------|------------------|
| get      | O(1)             |
| put      | O(1)             |
| expiry cleanup | O(log N)   |

---

## How to Run

```bash
g++ main.cpp LRU_cache.cpp -o cache
./cache
