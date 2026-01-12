#include "engine/engine.hpp"

Engine::Engine(): mem_storage(100),parser(""),encoder(),handler(),aof_persister(),exp([this](const std::string& key) { this->onExpire(key); }){};

Engine::~Engine(){
    //rdb_storage.save();
    //aof_storage.save();
};
void Engine::load_db(){
    //rdb_storage.load();
    aof_persister.load(mem_storage);
}
void Engine::onExpire(const std::string& key){
        mem_storage.erase(key);
        std::cout << "Expired: " << key << "\n";
}

std::string Engine::process(const char* request){
        parser.setInput(request);
        RESP data_structure=parser.parse();
        // Extract plain strings
        std::vector<std::string> parsed_cmd;
        for (auto &item : std::get<std::vector<RESP>>(data_structure.value)){
            parsed_cmd.push_back(std::get<std::string>(item.value));

        }
        // Execute
        std::string raw_resp(request);
        RESP reply =handler.execute(parsed_cmd,raw_resp,mem_storage,aof_persister,exp);
        return encoder.encode(reply);
}