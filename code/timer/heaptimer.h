#ifndef HEAP_TIMER_H
#define HEAP_TIMER_H

#include <queue>
#include <unordered_map>
#include <time.h>
#include <algorithm>
#include <arpa/inet.h> 
#include <functional> 
#include <assert.h> 
#include <chrono>
#include "../log/log.h"

typedef std::function<void()> TimeoutCallBack;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MS;
typedef Clock::time_point TimeStamp;

/**
 * 定时器结点，
*/
struct TimerNode {
    int id;
    TimeStamp expires; // 时间戳
    TimeoutCallBack cb;
    // 运算符重载，比较两个定时器节点的时间戳
    bool operator<(const TimerNode& t) {
        return expires < t.expires;
    }
};

/**
 * 定时器，小根堆实现，每个节点的数据都比其子节点小
*/
class HeapTimer {
public:
    HeapTimer() { heap_.reserve(64); }

    ~HeapTimer() { clear(); }
    
    void adjust(int id, int newExpires);

    void add(int id, int timeOut, const TimeoutCallBack& cb);

    void doWork(int id);

    void clear();

    void tick();

    void pop();

    int GetNextTick();

private:
    void del_(size_t i);
    
    void siftup_(size_t i);

    bool siftdown_(size_t index, size_t n);

    void SwapNode_(size_t i, size_t j);

    // 堆，保存定时器节点
    std::vector<TimerNode> heap_;

    // 哈希映射， 存储定时器节点位置
    std::unordered_map<int, size_t> ref_;
};

#endif //HEAP_TIMER_H