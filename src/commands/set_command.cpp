#include "commands/set_command.hpp"

RESP SetCommand::execute(const std::string& raw_resp_text,const std::vector<std::string>& args,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp) {
    if (args.empty()){
        std::string err="empty command";
        return _makeError(err);
    } 
    std::optional<RESP> wrong_type_operation_error;
    const std::string& cmd = args[0];
    if((cmd=="SADD"&& args.size()>=3)){
      auto value=db.get(args[1]);
      int count=0;
      if(!value.has_value()){
        value = std::unordered_set<std::string>();       
      }
      wrong_type_operation_error=check_type(SET,value.value());
      if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
      } 
      auto& set_data = std::get<std::unordered_set<std::string>>(value.value());
      for (size_t i = 2; i < args.size(); ++i){ 
        if(set_data.count(args[i])>0) continue;
        set_data.emplace(args[i]);
            count++;
        }
      aof_persister.appendCommand(raw_resp_text);
      db.put(args[1],set_data);
      return _makeInteger(count);
        
    }else if (cmd=="SMEMBERS" && args.size()==2){
        auto value=db.get(args[1]);
        if(!value.has_value()){
            return _makeSimpleString("(empty array)");
        }
        wrong_type_operation_error=check_type(SET,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto set_data = std::get<std::unordered_set<std::string>>(value.value());
        std::vector<RESP> response_array;
        for (auto &&element : set_data){   
            RESP temp_element;
            temp_element.type=RESP::SIMPLE_STRING;
            temp_element.value=element;
            response_array.emplace_back(temp_element);
        }
        return return_resp_array(response_array);

    }else if (cmd=="SISMEMBER" && args.size()==3){
        auto value = db.get(args[1]);
        if(value.has_value()){
            return _makeInteger(0);
        }
        wrong_type_operation_error=check_type(SET,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto set_data = std::get<std::unordered_set<std::string>>(value.value());
        auto hit = set_data.find(args[2]);
        if (hit != set_data.end()){
            return _makeInteger(1);
        }
        return _makeInteger(0);
                
    }else if (cmd=="SREM" && args.size()>=3){
        auto value = db.get(args[1]);
        if(!value.has_value()) return _makeInteger(0);
        wrong_type_operation_error=check_type(SET,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto& set_data = std::get<std::unordered_set<std::string>>(value.value());
        int deleted_count=0;
        int count=args.size();
        for (size_t i = 2; i < count; i++){
            if(set_data.erase(args[i])>0){
                deleted_count++;
            }
        }
        aof_persister.appendCommand(raw_resp_text);
        db.put(args[1],set_data);
        return _makeInteger(deleted_count);
    }else if(cmd=="SCARD" && args.size()==2){
        auto value = db.get(args[1]);
        if(!value.has_value()) return _makeInteger(0);
        wrong_type_operation_error=check_type(SET,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        std::unordered_set<std::string> set_data=std::get<std::unordered_set<std::string>>(value.value());
        return _makeInteger(set_data.size());
        
    }else if (cmd=="SMOVE" && args.size()==4){
        auto src=db.get(args[1]);
        auto dest=db.get(args[2]);
        std::string member=args[3];
        if(!(src.has_value() && dest.has_value())){
            return _makeInteger(0);
        }
        if(!(std::holds_alternative<std::unordered_set<std::string>>(src.value()) && std::holds_alternative<std::unordered_set<std::string>>(dest.value()))){
            return _makeError("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
        }
        std::unordered_set<std::string> source_set_data=std::get<std::unordered_set<std::string>>(src.value());
        std::unordered_set<std::string> destination_set_data=std::get<std::unordered_set<std::string>>(dest.value());
        if(source_set_data.count(member)<1){
            return _makeInteger(0);
        }
        source_set_data.erase(member);
        destination_set_data.emplace(member);
        aof_persister.appendCommand(raw_resp_text);
        db.put(args[2],destination_set_data);
        db.put(args[1],source_set_data);
        return _makeInteger(1);
    }else if (cmd=="SUNION" && args.size()>=3){
        std::unordered_set<std::string> temp_union_set;
        std::vector<RESP> union_set;
        for (size_t i = 1; i < args.size(); i++){
            auto value=db.get(args[i]);
            if (std::holds_alternative<std::unordered_set<std::string>>(value.value())){
                std::unordered_set<std::string> temp_set=std::get<std::unordered_set<std::string>>(value.value());
                for (auto &&i : temp_set){    
                     temp_union_set.emplace(i);
                    }   
                }else{
                    if(!std::holds_alternative<std::unordered_set<std::string>>(value.value()) && value.has_value()){
                        return _makeError("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
                    }
                continue;
            }
            
        }
        for (auto &&i : temp_union_set){
             RESP temp_item;
             temp_item.type=RESP::SIMPLE_STRING;
             temp_item.value=i;
             union_set.emplace_back(temp_item);
        }
        return return_resp_array(union_set);

    }else if (cmd=="SDIFF" && args.size()>=3){
        std::unordered_set<std::string> temp_diff_set;
        std::vector<RESP> diff_set;
        auto first_set=db.get(args[1]);
        bool found_first=false;
        for (size_t i = 1; i < args.size(); i++){
            auto value=db.get(args[i]);
            if (std::holds_alternative<std::unordered_set<std::string>>(value.value())){
                std::unordered_set<std::string> item_set=std::get<std::unordered_set<std::string>>(value.value());
                if(item_set.size()==0){
                    continue;
                }else if (item_set.size()>0 && found_first==false){
                    temp_diff_set=item_set;
                    found_first=true;
                    continue;
                }       
                std::unordered_set<std::string> temp;
                std::set_difference(temp_diff_set.begin(), temp_diff_set.end(),item_set.begin(), item_set.end(),std::inserter(temp, temp.begin()));
                temp_diff_set = temp;
            }else{
                if(!std::holds_alternative<std::unordered_set<std::string>>(value.value()) && value.has_value()){
                    return _makeError("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
                }
                continue;
        }
        }
        for (auto &&i : temp_diff_set){
            RESP temporary_item;
            temporary_item.type=RESP::SIMPLE_STRING;
            temporary_item.value=i;
            diff_set.emplace_back(temporary_item);
        }
        return return_resp_array(diff_set);
    }else if (cmd=="SINTER" && args.size()>=3){
        std::unordered_set<std::string> temp_intersect_set;
        std::vector<RESP> intersect_set;
        auto first_set=db.get(args[1]);
        bool found_first=false;
        for (size_t i = 1; i < args.size(); i++){
            auto value=db.get(args[i]);
            if (std::holds_alternative<std::unordered_set<std::string>>(value.value())){
                std::unordered_set<std::string> item_set=std::get<std::unordered_set<std::string>>(value.value());
                if(item_set.size()==0){
                    continue;
                }else if (item_set.size()>0 && found_first==false){
                    temp_intersect_set=item_set;
                    found_first=true;
                    continue;
                }
                        
                std::unordered_set<std::string> temp;
                std::set_intersection(temp_intersect_set.begin(), temp_intersect_set.end(),
                item_set.begin(), item_set.end(),
                std::inserter(temp, temp.begin()));
                temp_intersect_set = temp;
            }else{
                if(!std::holds_alternative<std::unordered_set<std::string>>(value.value()) && value.has_value()){
                    return _makeError("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
                }
                continue;
            }
        }
        for (auto &&i : temp_intersect_set){
            RESP temporary_item;
            temporary_item.type=RESP::SIMPLE_STRING;
            temporary_item.value=i;
            intersect_set.emplace_back(temporary_item);
        }
        
        return return_resp_array(intersect_set);
    }else{
        return _makeError("(error) wrong number of arguments for "+cmd+"\n");   
    }
}