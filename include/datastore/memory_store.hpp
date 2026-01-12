#pragma once
#include <utils/types.hpp>

class Memory_Store{
    private:        
            int capacity;
            int minFreq;
            std::unordered_map<std::string, std::list<Node>::iterator> keyIndex;
            std::unordered_map<int, std::list<Node>> freqList; 
    public:
          Memory_Store(size_t cap);
          bool contains(const std::string& key) const;
          std::optional<Value> get(const std::string& key);
          void put(const std::string& key, const Value& value);
          bool erase(const std::string& key);
          std::unordered_map<int,std::list<Node>>  get_freqList();
          std::unordered_map<std::string, typename std::list<Node>::iterator> get_keyIterator();
          Memory_Store clone();
          int size();
          void evict();
          void clear();
};

