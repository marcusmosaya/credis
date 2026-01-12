#include <datastore/memory_store.hpp>
#include <iostream>
Memory_Store::Memory_Store(size_t cap) : capacity(cap) {}

bool Memory_Store::contains(const std::string& key) const {
        return keyIndex.count(key);
}

std::optional<Value> Memory_Store::get(const std::string& key) {
    if (!contains(key)) return std::nullopt;

    auto it = keyIndex[key];
    int oldFreq = it->freq;    

    // move node to next frequency bucket
    Node node = *it;
    freqList[oldFreq].erase(it);

    if (freqList[oldFreq].empty()) {
        freqList.erase(oldFreq);
        if (minFreq == oldFreq)
            minFreq++;
    }

    node.freq++;
    freqList[node.freq].push_front(node);
    keyIndex[key] = freqList[node.freq].begin();

        return node.value;
}

void Memory_Store::put(const std::string& key, const Value& value) {
    if (capacity == 0) return;
    // Update existing key
    if (contains(key)) {
        auto it = keyIndex[key];
        it->value = value; // update value
        get(key); // bump frequency and LRU order
        return;
    }

    // Evict if full
    if (keyIndex.size() >= capacity) {
        evict();
    }

    // Insert new key at freq=1
    Node node{key, value, 1};
    freqList[1].push_front(node);
    keyIndex[key] = freqList[1].begin();
    minFreq = 1;
}

int Memory_Store::size(){
    return keyIndex.size();
}
void Memory_Store::evict() {
    // LFU eviction: find minFreq bucket
    auto& lst = freqList[minFreq];

    // RFU eviction: evict LRU inside lowest-frequency bucket
    auto it = lst.back(); // oldest entry in freq bucket

    std::string keyToRemove = it.key;
    lst.pop_back();

    if (lst.empty())
        freqList.erase(minFreq);
        keyIndex.erase(keyToRemove);
}
void Memory_Store::clear(){
        freqList.clear();
        keyIndex.clear();
}

bool Memory_Store::erase(const std::string& key){
    if (keyIndex.find(key) == keyIndex.end())
            return false;

        auto it = keyIndex[key];
        int freq = it->freq;

        // Remove node
        freqList[freq].erase(it);
        keyIndex.erase(key);

        // Fix minFreq if needed
        if (freqList[freq].empty()) {
            freqList.erase(freq);
            if (minFreq == freq)
                minFreq++;
        }

        return true;
}

std::unordered_map<int,std::list<Node>>  Memory_Store::get_freqList(){
    return freqList;
}

std::unordered_map<std::string, typename std::list<Node>::iterator> Memory_Store::get_keyIterator(){
    return keyIndex;
}

Memory_Store Memory_Store::clone(){
    Memory_Store copy_store(capacity);
    copy_store.freqList=freqList;
    copy_store.keyIndex=keyIndex;
    return copy_store;
}