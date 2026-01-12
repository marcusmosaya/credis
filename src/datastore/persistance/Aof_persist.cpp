#include "datastore/persistance/Aof_persist.hpp"
#include <filesystem>
bool AOF_Persist::load(Memory_Store& db) {   
    //To work on
    return true;
}

// Save entire DB to AOF (rewrite)
bool AOF_Persist::save(Memory_Store& db) {
    std::unordered_map<std::string, typename std::list<Node>::iterator> map_to_traverse;
    map_to_traverse=db.get_keyIterator();
    std::lock_guard<std::mutex> lock(mu);
    std::string temp_filename=filename+".temp";
    std::ofstream temp(temp_filename,std::ios::trunc);

    if (!temp.is_open()) return false;
    for (auto &&[key,value] : map_to_traverse){
        auto& data=value->value;
        std::string raw_text=generate_resp_text(data,value->key);
        temp<<raw_text;
    }
    std::filesystem::rename(temp_filename,filename);
    return true;
}

bool AOF_Persist::appendCommand(const std::string& cmd){
    if (isLoading==true){ 
        return false;
    }
    std::lock_guard<std::mutex> lock(mu);
    std::ofstream out(filename, std::ios::app);
   
    if (!out.is_open()) return false;
    out << cmd ;
    return true;
}

bool AOF_Persist::getIsloading() const{
    return isLoading;
}

void AOF_Persist::setIsloading(bool loading){
    isLoading=loading;
}


