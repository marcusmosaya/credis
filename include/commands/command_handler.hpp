#pragma once
#include <utils/types.hpp>
#include <utils/functions.hpp>
#include <network/Resp.hpp>
#include <datastore/memory_store.hpp>
#include <datastore/expire.hpp>
#include <datastore/persistance/persistance.hpp>
#include <datastore/persistance/Aof_persist.hpp>

class CommandHandler {
public:
    using StrValue = std::string;
    using HashValue = std::unordered_map<std::string, std::string>;
    using ListValue = std::vector<std::string>;
    using SetValue=std::unordered_set<std::string>;
    using SSetValue=sset;
    using Value = std::variant<StrValue, HashValue, ListValue,SetValue,SSetValue >;
    RESP execute(const std::vector<std::string>& parts,std::string& raw_resp_text,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp);
    void execute_for_load_aof(const std::vector<std::string>& parts);
    void set_db(Memory_Store& database);
    void set_expire_manager(Expire_Manager& expire_manager);
    void onExpired(const std::string& key);
private:
    RESP handleStrCommand(const std::vector<std::string>& args,std::string& raw_resp_text,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp);
    RESP handleKeyCommand(const std::vector<std::string>& args,std::string& raw_resp_text,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp);
    RESP handleServerCommand(const std::vector<std::string>& args,std::string& raw_resp_text,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp);
    RESP handleHashCommand(const std::vector<std::string>& args,std::string& raw_resp_text,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp);
    RESP handleListCommand(const std::vector<std::string>& args,std::string& raw_resp_text,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp);
    RESP handleSetCommand(const std::vector<std::string>& args,std::string& raw_resp_text,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp);
    RESP handleSSetCommand(const std::vector<std::string>& args,std::string& raw_resp_text,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp);
    std::vector<std::string> split(const std::string& s);
};
