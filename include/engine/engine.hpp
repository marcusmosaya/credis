#pragma once
#include <network/Resp.hpp>
#include <commands/command_handler.hpp>
#include <datastore/memory_store.hpp>
#include <datastore/persistance/Aof_persist.hpp>
#include <datastore/expire.hpp>
#include <string>
#include <utils/functions.hpp>
class Engine
{
private:
    Memory_Store mem_storage;
    CommandHandler handler;
    RESPParser parser;
    RESPGenerator encoder;
    AOF_Persist aof_persister;
    Expire_Manager exp;

public:
    Engine();
    ~Engine();
    void load_db();
    std::string process(const char* request);
    void onExpire(const std::string& key);

};
