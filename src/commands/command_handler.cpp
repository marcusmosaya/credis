#include "commands/command_handler.hpp"
#include "commands/string_command.hpp"
#include "commands/hash_command.hpp"
#include "commands/list_command.hpp"
#include "commands/keys_command.hpp"
#include "commands/server_command.hpp"
#include "commands/hash_command.hpp"
#include "commands/set_command.hpp"
#include "commands/sset_command.hpp"
std::vector<std::string> CommandHandler::split(const std::string& s) {
    std::istringstream iss(s);
    std::vector<std::string> parts;
    std::string part;
    while (iss >> part) parts.push_back(part);
    return parts;
}

RESP CommandHandler::execute(const std::vector<std::string>& parts,std::string& raw_resp_text,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp) {
    if (parts.empty()){        
        std::string err="empty command";
        return _makeError(err);
    }  
    

    std::string cmd = parts[0];

    if (is_string_command(cmd)) {
        
        return handleStrCommand(parts,raw_resp_text,db,aof_persister,exp);

    } else if(is_hash_command(cmd)) {
        
        return handleHashCommand(parts,raw_resp_text,db,aof_persister,exp);
    } else if (is_list_command(cmd)) {
        
        return handleListCommand(parts,raw_resp_text,db,aof_persister,exp);

    }else if(is_keys_command(cmd)){
        
        return handleKeyCommand(parts,raw_resp_text,db,aof_persister,exp);

    } else if(is_set_command(cmd)){
        
        return handleSetCommand(parts,raw_resp_text,db,aof_persister,exp);

    }else if(is_sorted_set_command(cmd)){
        
        return handleSSetCommand(parts,raw_resp_text,db,aof_persister,exp);

    }else if(is_pub_sub_command(cmd)){
        //To handle pub/sub- not yet
        return handleStrCommand(parts,raw_resp_text,db,aof_persister,exp);
        
    }else if (is_transactions_command(cmd)){
        //To handle transactions- not yet
        return handleStrCommand(parts,raw_resp_text,db,aof_persister,exp);

    }else if (is_server_command(cmd)){
        //To handle server- not yet
        return handleServerCommand(parts,raw_resp_text,db,aof_persister,exp);

    }else {
        return _makeError("(error) unknown command: "+cmd);
    }
}
void CommandHandler::execute_for_load_aof(const std::vector<std::string>& parts){
   if (parts.empty()){        
        std::string err="empty command";
        _makeError(err);
    }  
    std::string cmd = parts[0];
}



void CommandHandler::set_expire_manager(Expire_Manager& expire_manager){
    return;
}
void CommandHandler::onExpired(const std::string& key){
    return;
}
RESP CommandHandler::handleStrCommand(const std::vector<std::string>& args,std::string& raw_resp_text,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp) {
    return StrCommand::execute(raw_resp_text,args,db,aof_persister,exp);
}
RESP CommandHandler::handleKeyCommand(const std::vector<std::string>& args,std::string& raw_resp_text,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp){
    return KeyCommand::execute(raw_resp_text,args,db,aof_persister,exp);
}

RESP CommandHandler::handleServerCommand(const std::vector<std::string>& args,std::string& raw_resp_text,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp){
    return ServerCommand::execute(raw_resp_text,args,db,aof_persister,exp);
}

RESP CommandHandler::handleHashCommand(const std::vector<std::string>& args,std::string& raw_resp_text,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp) {
    return HashCommand::execute(raw_resp_text,args,db,aof_persister,exp);
}

RESP CommandHandler::handleListCommand(const std::vector<std::string>& args,std::string& raw_resp_text,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp) {
   return ListCommand::execute(raw_resp_text,args,db,aof_persister,exp);
}

RESP CommandHandler::handleSetCommand(const std::vector<std::string>& args,std::string& raw_resp_text,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp){
    return SetCommand::execute(raw_resp_text,args,db,aof_persister,exp);
}

RESP CommandHandler::handleSSetCommand(const std::vector<std::string>& args,std::string& raw_resp_text,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp){
    return SSetCommand::execute(raw_resp_text,args,db,aof_persister,exp);
}