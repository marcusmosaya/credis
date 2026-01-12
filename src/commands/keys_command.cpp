#include "commands/keys_command.hpp"

RESP KeyCommand::execute(const std::string& raw_resp_text,const std::vector<std::string>& args,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp) {
      if (args.empty()){
        std::string err="empty command";
        return _makeError(err);
    } 
    const std::string& cmd = args[0];
     if (cmd == "EXISTS" && args.size() >= 2) {
        int count=args.size();
        int key_count=0;
        for (size_t i = 1; i < count; i++)
        {
             auto value = db.get(args[i]);
             if (value.has_value()) {
                 key_count++;
             }
        }
    
        return _makeInteger(key_count);
    }else if(cmd=="EXPIRE" && args.size()==3){
        if(!is_integer(args[2])){
           return _makeError("(error) ERR value is not an integer or out of range"); 
        }
        auto value = db.get(args[1]);
        if(!value.has_value()){
            return _makeInteger(0);
        }
        aof_persister.appendCommand(raw_resp_text);
        exp.setExpire(args[1],std::stoi(args[2]));
        return _makeInteger(1);        
        }else if (cmd=="TTL" && args.size()==2){
            auto value = db.get(args[1]);
            if (value.has_value()) {
                return _makeInteger(exp.ttl(args[1]));
            }
            return _makeInteger(-2);
            
        }else if (cmd=="PERSIST"&& args.size()==2){   
            auto value = db.get(args[1]);
            if (value.has_value()){
                aof_persister.appendCommand(raw_resp_text); 
                return _makeInteger(exp.persist(args[1]));
            }
            return _makeInteger(0);
            
        }else if(cmd=="RENAME" && args.size()==3){
         auto value = db.get(args[1]);
            if (!value.has_value()) return _makeError("(error) ERR no such key");  
            
            Value data=value.value();
            db.erase(args[1]);
            exp.persist(args[1]);
            aof_persister.appendCommand(raw_resp_text);
            db.put(args[2],data);
            return _makeSimpleString("OK");
            
        }else if (cmd == "DEL" && args.size() >= 2) {
        int delete_count=0;
        int count=args.size();
        aof_persister.appendCommand(raw_resp_text);
        for (size_t i = 1; i < count; i++)
        {
            if(db.erase(args[i])){
                delete_count++;
            }
        }
        return _makeInteger(delete_count);
    }
        else{
        return _makeError("(error) wrong number of arguments for "+cmd+"\n");
    }

}