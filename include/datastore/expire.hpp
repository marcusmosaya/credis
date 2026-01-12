#pragma once
#include <utils/types.hpp>
#include <queue>
#include <chrono>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class Expire_Manager {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    // Callback: called when a key must be deleted
    using ExpireCallback = std::function<void(const std::string&)>;
    explicit Expire_Manager(ExpireCallback cb);
    ~Expire_Manager();

    void stop();

    // Set TTL for a key
    void setExpire(const std::string& key, int ttlSeconds);

    // Remove expiration
    bool persist(const std::string& key);

    // Get TTL remaining (seconds)
    int ttl(const std::string& key);

    bool hasExpiration(const std::string& key);
private:
    struct ExpireEntry {
        std::string key;
        TimePoint expireAt;

        bool operator>(const ExpireEntry& other) const {
            return expireAt > other.expireAt;
        }
    };

    std::priority_queue<
        ExpireEntry,
        std::vector<ExpireEntry>,
        std::greater<ExpireEntry>
    > minHeap; 
    std::unordered_map<std::string, TimePoint> table;
    std::mutex mu;
    std::condition_variable cv;
    std::thread worker;
    std::atomic<bool> running{true};
    ExpireCallback callback;
    void run();    
};
