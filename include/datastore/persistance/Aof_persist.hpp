#pragma once
#include "persistance.hpp"
#include <network/Resp.hpp>

class AOF_Persist : public IPersistence {
private:
    std::string filename;
    std::mutex mu;
    bool isLoading;
public:
    AOF_Persist(const std::string& file="credis.aof")
        : filename(file),isLoading(false) {}
    bool load(Memory_Store& db) override;
    bool save(Memory_Store& db) override;
    void setIsloading(bool loading);
    bool getIsloading() const;
    bool appendCommand(const std::string& cmd);
};