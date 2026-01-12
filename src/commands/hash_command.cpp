#include "commands/hash_command.hpp"

RESP HashCommand::execute(const std::string& raw_resp_text,const std::vector<std::string>& args,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp) {
    if (args.empty()){
        std::string err="(Error)empty command";
        return _makeError(err);
    }
    std::optional<RESP> wrong_type_operation_error;
    const std::string& cmd = args[0];

    if (cmd == "HSET" && args.size() >= 4) {
        auto value = db.get(args[1]);
        if(args.size()%2!=0){
            return _makeError("(error) ERR wrong number of arguments for 'hset' command");
        }
        if (!value.has_value()){
            std::unordered_map<std::string, std::string> val;
            value=val;
        }
        wrong_type_operation_error=check_type(HTABLE,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        }   
        auto& hash_data = std::get<std::unordered_map<std::string, std::string>>(value.value());
        int count=args.size();
        for (size_t i = 2; i < count; i=i+2){
                hash_data[args[i]] = args[i+1];
        }
        aof_persister.appendCommand(raw_resp_text);
        db.put(args[1],hash_data);
        return _makeSimpleString("OK");
    } else if (cmd == "HGET" && args.size() == 3) {
        auto value = db.get(args[1]);
        if(!value.has_value()){
            return _makeSimpleString("(nil)");
        }
        wrong_type_operation_error=check_type(HTABLE,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto& hash_data = std::get<std::unordered_map<std::string, std::string>>(value.value());
        auto hit = hash_data.find(args[2]);
        
        if (hit != hash_data.end()) return _makeSimpleString(hit->second);
        
        return _makeSimpleString("(nil)");
    } else if (cmd == "HDEL" && args.size() >= 3) {
        auto value = db.get(args[1]);
        if(!value.has_value()){
            return _makeInteger(0);
        }
        wrong_type_operation_error=check_type(HTABLE,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto& hash_data = std::get<std::unordered_map<std::string, std::string>>(value.value());
        int deleted_count=0;
        int count=args.size();
        for (size_t i = 2; i < count; i++){
            if(hash_data.erase(args[i])){
                auto it = db.get(args[1]);
                deleted_count++;
            }
        }
        aof_persister.appendCommand(raw_resp_text);
        db.put(args[1],hash_data);
        return _makeInteger(deleted_count);

    }else if (cmd=="HGETALL" && args.size() == 2){
        auto value=db.get(args[1]);
        if(!value.has_value()){
            return _makeSimpleString("(nil)");
        }
        wrong_type_operation_error=check_type(HTABLE,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto& hash_data = std::get<std::unordered_map<std::string, std::string>>(value.value());
        std::vector<RESP> array;
        for (auto &&[k,v] : hash_data){   
            RESP temp_key,temp_value;
            temp_key.type=RESP::SIMPLE_STRING;
            temp_key.value=k;
            temp_value.type=RESP::SIMPLE_STRING;
            temp_value.value=v; 
            array.insert(array.begin(),temp_value);                       
            array.insert(array.begin(),temp_key);       
        }
        return return_resp_array(array);
    }else if (cmd=="HMGET" && args.size()>=3){
        auto value = db.get(args[1]);
        if(!value.has_value()){
            return _makeSimpleString("(nil)");
        }
        wrong_type_operation_error=check_type(HTABLE,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto& hash_data = std::get<std::unordered_map<std::string, std::string>>(value.value());
        std::vector<RESP>  array;
        int args_count=args.size();
        for (size_t i = 2; i < args_count; i++){
            auto hit = hash_data.find(args[i]);
            RESP data;
            data.type=RESP::SIMPLE_STRING;
            if (hit != hash_data.end()){
                data.value=hit->second;
            }else { 
                data.value="(nil)";
            }
            array.emplace_back(data);
        }                
        return return_resp_array(array);               
        
    }else if (cmd=="HINCRBY" && args.size()==4){
        auto value = db.get(args[1]);
        if(!is_integer(args[3])){
            return _makeError("(error) ERR value is not an integer or out of range");
        }
        if(!value.has_value()){
            value=std::unordered_map<std::string,std::string>();
        }
        wrong_type_operation_error=check_type(HTABLE,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto hash_data = std::get<std::unordered_map<std::string, std::string>>(value.value());
        auto hit = hash_data.find(args[2]);
        if (hit != hash_data.end()){
            hash_data[args[2]]=std::to_string(std::stoi(hit->second)+std::stoi(args[3]));
        }else{ 
            hash_data[args[2]]=args[3];
        }
        aof_persister.appendCommand(raw_resp_text);
        db.put(args[1],hash_data);
        return _makeInteger(std::stoi(hash_data[args[2]]));

    }else if (cmd=="HEXISTS" && args.size()==3){
            auto value = db.get(args[1]);

            if(!value.has_value()){
                return _makeInteger(0);
            }
            wrong_type_operation_error=check_type(HTABLE,value.value());
            if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
            } 
            auto hash_data = std::get<std::unordered_map<std::string, std::string>>(value.value());
            auto hit = hash_data.find(args[2]);
            if (hit != hash_data.end()){
                return _makeInteger(1);
            }
            return _makeInteger(0);
    }else if(cmd=="HKEYS" && args.size()==2){
            auto value = db.get(args[1]);
            if(!value.has_value()){
                return _makeSimpleString("(empty array)");
            }
            wrong_type_operation_error=check_type(HTABLE,value.value());
            if(wrong_type_operation_error.has_value()){
                return wrong_type_operation_error.value();
            } 
            auto hash_data = std::get<std::unordered_map<std::string, std::string>>(value.value());
            auto hit = hash_data.find(args[1]);
            std::vector<RESP> keys;
            for (auto &&[key,value] : hash_data)
            {
                RESP temp_key;
                temp_key.type= RESP::SIMPLE_STRING;
                temp_key.value=key;
                keys.insert(keys.begin(),temp_key);
                
            }
            return return_resp_array(keys);
                
    }else if (cmd=="HVALS" && args.size()==2){
            auto value = db.get(args[1]);
            if(!value.has_value()){
                return _makeSimpleString("(empty array)");
            }
            wrong_type_operation_error=check_type(HTABLE,value.value());
            if(wrong_type_operation_error.has_value()){
               return wrong_type_operation_error.value();
            } 
            auto& hash_data = std::get<std::unordered_map<std::string, std::string>>(value.value());
            auto hit = hash_data.find(args[1]);
            std::vector<RESP> vals;
            for (auto &&[key,value] : hash_data)
            {
                RESP temp_val;
                temp_val.type= RESP::SIMPLE_STRING;
                temp_val.value=value;
                vals.insert(vals.begin(),temp_val);   
            }
            return return_resp_array(vals);    
    }else if (cmd=="HLEN" && args.size()==2){
        auto value = db.get(args[1]);
        if(!value.has_value()){
            return _makeInteger(0);
        }
        wrong_type_operation_error=check_type(HTABLE,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto hash_data = std::get<std::unordered_map<std::string, std::string>>(value.value());
        return _makeInteger(hash_data.size());        
    }
     else {
        return _makeError("(error) wrong number of arguments for "+ cmd );
    }
}

