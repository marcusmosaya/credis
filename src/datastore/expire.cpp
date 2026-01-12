#include "datastore/expire.hpp"
      void Expire_Manager::run() {
        std::unique_lock<std::mutex> lock(mu);

        while (running) {
            if (minHeap.empty()) {
                cv.wait(lock);
            } else {
                auto now = Clock::now();
                auto nextExpire = minHeap.top().expireAt;

                if (now >= nextExpire) {
                    // Expire this key
                    auto entry = minHeap.top();
                    minHeap.pop();

                    // Check if key was overwritten later
                    auto it = table.find(entry.key);
                    if (it != table.end() && it->second == entry.expireAt) {
                        // Expire for real
                        std::string expiredKey = entry.key;
                        table.erase(it);

                        lock.unlock();
                        callback(expiredKey);
                        lock.lock();
                    }
                } else {
                    // Sleep until the next expiration
                    cv.wait_until(lock, nextExpire);
                }
            }
        }
    }

    Expire_Manager::Expire_Manager(ExpireCallback cb) : callback(cb) {
        worker = std::thread([this] { run(); });
    }

    Expire_Manager::~Expire_Manager(){
        stop();
    }

    void Expire_Manager::stop(){
         if (!running.exchange(false))
            return;
        cv.notify_all();
        if (worker.joinable())
            worker.join();
    }

    // Set TTL for a key
    void Expire_Manager::setExpire(const std::string& key, int ttlSeconds){
        std::lock_guard<std::mutex> lock(mu);
        TimePoint tp = Clock::now() + std::chrono::seconds(ttlSeconds);
        table[key] = tp;
        minHeap.push({key, tp});
        cv.notify_all();

    }

    // Remove expiration
    bool Expire_Manager::persist(const std::string& key){
        std::lock_guard<std::mutex> lock(mu);
        return table.erase(key);
        // heap will still have old entries, but ignored later
    }
    
    // Get TTL remaining (seconds)
    int Expire_Manager::ttl(const std::string& key){
         std::lock_guard<std::mutex> lock(mu);

        if (!table.count(key)) return -1;

        auto now = Clock::now();
        auto tp = table[key];

        auto diff = std::chrono::duration_cast<std::chrono::seconds>(tp - now);
        return diff.count();
    }

    bool Expire_Manager::hasExpiration(const std::string& key){
        std::lock_guard<std::mutex> lock(mu);
        return table.count(key);
    }