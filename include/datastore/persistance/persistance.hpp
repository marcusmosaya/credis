#pragma once
#include <utils/types.hpp>
#include <fstream>
#include <mutex>
#include <datastore/memory_store.hpp>
#include <utils/functions.hpp>

class IPersistence {
public:
    virtual ~IPersistence() = default;

    // Load data from disk into memory
    virtual bool load(Memory_Store& db) = 0;

    // Persist data to disk (snapshot or log append)
    virtual bool save(Memory_Store& db) = 0;
};