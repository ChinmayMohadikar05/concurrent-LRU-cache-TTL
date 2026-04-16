## LRU Cache with TTL (C++)

## Overview
A high-performance in-memory cache system implementing:
- LRU (Least Recently Used) eviction
- TTL (Time-To-Live) expiration
- Concurrent access handling
- Background cleaner thread

## Features
- O(1) get and put operations
- TTL-based automatic expiration
- Min-heap for efficient expiry tracking
- Thread-safe using mutex
- Background cleanup thread

## System Design

### Components:
- Doubly Linked List → to maintains LRU order
- Hash Map → for O(1) access
- latestExpiry map → to avoid stale heap entries 
- Min Heap → for efficient TTL expiry
- Background Thread → for active cleanup
- Thread safety → use mutex for safe concurrent access

## Complexity
| Operation | Time Complexity |
|----------|------------------|
| get      | O(1)             |
| put      | O(1)             |
| expiry cleanup | O(log N)   |

## Performance
Handles 1M operations/sec (single-threaded)

## How to Run

```bash
g++ main.cpp LRU_cache.cpp -o cache
./cache
