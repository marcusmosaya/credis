#pragma once
#include <iostream>
#include <utils/types.hpp>
#include <network/Resp.hpp>
#include <algorithm>
#include <utils/functions.hpp>
#include <datastore/memory_store.hpp>
#include <datastore/expire.hpp>
#include <datastore/persistance/Aof_persist.hpp>
namespace SetCommand {
    RESP execute(const std::string& raw_resp_text,const std::vector<std::string>& args,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp);
}