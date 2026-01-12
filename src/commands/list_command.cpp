#include "commands/list_command.hpp"

RESP ListCommand::execute(const std::string& raw_resp_text,const std::vector<std::string>& args,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp) {
    if (args.empty()){
        std::string err="empty command";
        return _makeError(err);
    }
    std::optional<RESP> wrong_type_operation_error;
    const std::string& cmd = args[0];
    if (cmd == "LPUSH" && args.size() >= 3) {
        auto value = db.get(args[1]);
        if(!value.has_value()){
            value = std::vector<std::string>();
        }
        wrong_type_operation_error=check_type(LIST,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto& list_data = std::get<std::vector<std::string>>(value.value());
        for (size_t i = 2; i < args.size(); ++i){ 
            list_data.insert(list_data.begin(), args[i]);
        }
        aof_persister.appendCommand(raw_resp_text);
        db.put(args[1],list_data);
        return _makeSimpleString("OK");
    } else if (cmd == "RPOP" && args.size() == 2) {
        auto value = db.get(args[1]);
        if(!value.has_value()){
            return _makeSimpleString("(nil)");
        }
        wrong_type_operation_error=check_type(LIST,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto& list_data = std::get<std::vector<std::string>>(value.value());
        if (!list_data.empty()) {
            std::string element= list_data.back();
            list_data.pop_back();
            return _makeSimpleString(element);
        }
        return _makeSimpleString("(nil)");
    } else if (cmd == "LRANGE" && args.size() == 4) {
        auto value = db.get(args[1]);
        if(!value.has_value()){
            return _makeSimpleString("(empty string)");
        }
        wrong_type_operation_error=check_type(LIST,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto& list_data = std::get<std::vector<std::string>>(value.value());
        int start = std::stoi(args[2]);
        int end = std::stoi(args[3]);
        end = std::min<int>(end, list_data.size() - 1);
        std::vector <RESP> response_vector_data;
        for (int i = start; i <= end && i < (int)list_data.size(); ++i){
            RESP temp;
            temp.type=RESP::SIMPLE_STRING;
            temp.value=list_data[i];
            response_vector_data.push_back(temp);
        }
        
        return return_resp_array(response_vector_data);
    }else if (cmd=="RPUSH" && args.size()>=3){
        auto value = db.get(args[1]);
        if(!value.has_value()){
            value = std::vector<std::string>();
        }
        wrong_type_operation_error=check_type(LIST,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto& list_data = std::get<std::vector<std::string>>(value.value());
        for (size_t i = 2; i < args.size(); ++i){ 
            list_data.emplace_back(args[i]);
        }
        aof_persister.appendCommand(raw_resp_text);
        db.put(args[1],list_data);
        return _makeSimpleString("OK");    
    }else if (cmd=="LPOP" && args.size()==2){
        auto value = db.get(args[1]);
        if(!value.has_value()){
            value = std::vector<std::string>();
        }
        wrong_type_operation_error=check_type(LIST,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto list_data = std::get<std::vector<std::string>>(value.value());
        if (!list_data.empty()) {
            std::string element= list_data.at(0);
            list_data.erase(list_data.begin());
            return _makeSimpleString(element);
        }
        return _makeSimpleString("(nil)");
    }else if (cmd=="LINDEX" && args.size()==3){
        auto value = db.get(args[1]);
        if(!value.has_value()) return _makeSimpleString("(nil)");
        wrong_type_operation_error=check_type(LIST,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto& list_data=std::get<std::vector<std::string>>(value.value());
        int index=std::stoi(args[2]);
        if(index>list_data.size()-1||index<0){
            return _makeSimpleString("(nil)");
        }
        return _makeSimpleString(list_data[index]);
    }else if (cmd=="LLEN" && args.size()==2){
        auto value = db.get(args[1]);
        if(!value.has_value()) _makeInteger(0);
        wrong_type_operation_error=check_type(LIST,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        std::vector<std::string> list_data=std::get<std::vector<std::string>>(value.value());
        return _makeInteger(list_data.size());
        
    }else {
        return _makeError("(error) wrong number of arguments for "+cmd+"\n");
    }
}