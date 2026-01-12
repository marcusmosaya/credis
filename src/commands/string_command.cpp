#include "commands/string_command.hpp"

RESP StrCommand::execute(const std::string& raw_resp_text,const std::vector<std::string>& args,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp) {
 
    if (args.empty()){
        std::string err="empty command";
        return _makeError(err);
    } 
    std::optional<RESP> wrong_type_operation_error;
    const std::string& cmd = args[0];
    if (cmd == "SET" && args.size() == 3) {
        auto value=db.get(args[1]);
        if(!value.has_value()){
        aof_persister.appendCommand(raw_resp_text);
        db.put(args[1],args[2]);
        return _makeSimpleString("OK");
        }
        wrong_type_operation_error=check_type(STRING,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        if (std::get<std::string>(value.value())!=args[2]){
               aof_persister.appendCommand(raw_resp_text);
        }
        
        db.put(args[1],args[2]);
        return _makeSimpleString("OK");
    }else if(cmd=="SETEX" && args.size()==4){
        auto value=db.get(args[1]);
        if(!value.has_value()){
            std::string val;
            value=val;  
        }
        wrong_type_operation_error=check_type(STRING,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        if(is_integer(args[2])){
            db.put(args[1],args[3]);
            exp.setExpire(args[1],std::stoi(args[2]));
            return _makeSimpleString("OK");
        }
        return _makeError("(error) Invalid ttl "+ args[2]+"; should be a non-negative integer"+ "\n");
    }else if(cmd=="MSET" && args.size()>=3){
        if((args.size()%2==1)){
            return _makeError("(error) wrong number of arguments for "+cmd+"\n");
        }
        int count=args.size();
        aof_persister.appendCommand(raw_resp_text);      
        for (size_t i = 1; i < count; i=i+2){
            db.put(args[i],args[i+1]);
        }
        return _makeSimpleString("OK");
    }else if(cmd=="APPEND" & args.size()==3){
        auto value = db.get(args[1]);
        //Quick fix on this parts.
        if(!value.has_value()){
            aof_persister.appendCommand(raw_resp_text);
            db.put(args[1],args[2]);
            return _makeInteger(args[2].size());
        }
        wrong_type_operation_error=check_type(STRING,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        std::string str_data=std::get<std::string>(value.value());
        str_data=str_data.append(args[2]);
        aof_persister.appendCommand(raw_resp_text);
        db.put(args[1],str_data);
        return _makeInteger(str_data.length());       
    }else if(cmd=="INCRBY" && args.size()==3){
        if (!is_integer(args[2])){
            return _makeError("(error) ERR value is not an integer or out of range.\n");
        }
        auto value = db.get(args[1]);
        int count=std::stoi(args[2]);
        if(!value.has_value()){
             aof_persister.appendCommand(raw_resp_text);
             db.put(args[1],std::to_string(count));
            return _makeInteger(std::stoi(args[2]));      
        }
        wrong_type_operation_error=check_type(STRING,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto str_data=std::get<std::string>(value.value());
        if(!is_integer(str_data)){
            return _makeError("(error) ERR value is not an integer or out of range.\n");
        }
        count=std::stoi(str_data);
        count=count+std::stoi(args[2]);
        aof_persister.appendCommand(raw_resp_text);
        db.put(args[1],std::to_string(count));
        return _makeInteger(count);

    }else if(cmd=="DECRBY" && args.size()==3){
     if (!is_integer(args[2])){
            return _makeError("(error) ERR value is not an integer or out of range.\n");
        }
        auto value = db.get(args[1]);
        if(!value.has_value()){
             db.put(args[1],std::to_string(-std::stoi(args[2])));
            return _makeInteger(-std::stoi(args[2]));      
        }
        wrong_type_operation_error=check_type(STRING,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto str_data=std::get<std::string>(value.value());
         if(!is_integer(str_data)){
            return _makeError("(error) ERR value is not an integer or out of range.\n");
        }
        int count=std::stoi(str_data);
        count=count-std::stoi(args[2]);
        aof_persister.appendCommand(raw_resp_text);
        db.put(args[1],std::to_string(count));
        return _makeInteger(count);
    }else if(cmd=="INCR" && args.size()==2){
        auto value = db.get(args[1]);
        if(!value.has_value()){
            aof_persister.appendCommand(raw_resp_text);
            db.put(args[1],std::to_string(1));
            return _makeInteger(1);
        }
        wrong_type_operation_error=check_type(STRING,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        std::string str_data=std::get<std::string>(value.value());
        if(!is_integer(str_data)){
                return _makeSimpleString("(error) ERR value is not an integer or out of range");   
        }
        int count=std::stoi(str_data);
        count++;
        aof_persister.appendCommand(raw_resp_text);
        db.put(args[1],std::to_string(count));
        return _makeInteger(count);
    }else if(cmd=="DECR" && args.size()==2){
         auto value = db.get(args[1]);
        if(!value.has_value()){
            aof_persister.appendCommand(raw_resp_text);
            db.put(args[1],std::to_string(-1));
            return _makeInteger(-1);
        }
        wrong_type_operation_error=check_type(STRING,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        std::string str_data=std::get<std::string>(value.value());
        if(!is_integer(str_data)){
            return _makeSimpleString("(error) ERR value is not an integer or out of range");   
        }
        int count=std::stoi(str_data);
        count--;
        aof_persister.appendCommand(raw_resp_text);
        db.put(args[1],std::to_string(count));
        return _makeInteger(count);

    }
    else if (cmd == "GET" && args.size() == 2) {
        auto value = db.get(args[1]);
        if(!value.has_value()){
            return _makeBulkString("(nil)");
        }
        wrong_type_operation_error=check_type(STRING,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        aof_persister.save(db);
        return _makeSimpleString(std::get<std::string>(value.value()));

    }  else {
        return _makeError("(error) wrong number of arguments for "+cmd+"\n");
    }
}