#include "commands/sset_command.hpp"

RESP SSetCommand::execute(const std::string& raw_resp_text,const std::vector<std::string>& args,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp){
     if (args.empty()){
        std::string err="empty command";
        return _makeError(err);
    } 
    std::optional<RESP> wrong_type_operation_error;
    const std::string& cmd = args[0];
    if((cmd=="ZADD"&& args.size()>=3)){
      if(args.size()%2!=0){
        return _makeError("(error) ERR syntax error.");
      }
      auto value=db.get(args[1]);
      int count=0;
      if(!value.has_value()){
        value = sset();       
      }
        wrong_type_operation_error=check_type(SSET,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto sset_data = std::get<sset>(value.value());
        for (size_t i = 2; i < args.size(); i=i+2){ 
            if(sset_data.consists(args[i+1])){
                sset_data.put(args[i+1],std::stod(args[i]));
                continue;
            }
            sset_data.put(args[i+1],std::stod(args[i]));
            count++;
        }
        aof_persister.appendCommand(raw_resp_text);
        db.put(args[1],sset_data);
        return _makeInteger(count);

    }else if (cmd=="ZRANGE" && (args.size()>=4 && args.size()<=10)){
        //std::unordered_set<std::string> allowed_flags={"WITHSCORE","LIMIT","REV"};

        if(!(is_integer(args[2])&&is_integer(args[3]))){
            return _makeError("(error) ERR value not an integer or out of range.");
        }
        if(!(std::stoi(args[2])>=0 && std::stoi(args[3])>=0)){
            return _makeError("(error) ERR value not an integer or out of range.");
        }
        auto value=db.get(args[1]);
        if(!value.has_value()){
            return _makeSimpleString("(empty array)");
        }
        wrong_type_operation_error=check_type(SSET,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto sset_data=std::get<sset>(value.value());
        auto members=sset_data.range(std::stoi(args[2]),std::stoi(args[3]));
        std::vector<RESP> response_array;
        for (auto &&i : members){
            RESP item;
            item.type=RESP::SIMPLE_STRING;
            item.value=i.first;
            response_array.push_back(item);
        }
        
        return return_resp_array(response_array);

    }else if (cmd=="ZRANK" && (args.size()>=3 && args.size()<5)){
        auto value=db.get(args[1]);
        if(!value.has_value()){
            return _makeSimpleString("(nil)");
        }
        wrong_type_operation_error=check_type(SSET,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto sset_data=std::get<sset>(value.value());
        auto rank=sset_data.rank(args[2]);
        if(!rank.has_value()){
            return _makeSimpleString("(nil)");
        }
        auto values=rank.value();
        std::vector<RESP> response_array;
        if (args.size()==4){
            if(args[3]=="WITHSCORE"){
                RESP rank_,score_;
                rank_.type=RESP::INTEGER;
                score_.type=RESP::SIMPLE_STRING;
                for (auto &&i : values){
                    rank_.value=i.first;
                    score_.value= std::to_string(i.second);
                }
                response_array.push_back(rank_);
                response_array.push_back(score_);
                return return_resp_array(response_array);
            }
            return _makeError("(error) ERR syntax error.");
        }
        int rank_;
        for (auto &&i : values){
                rank_=i.first;
        }
        return _makeInteger(rank_);
    }else if (cmd=="ZINCRBY" && args.size()==4){
        if(!is_float(args[2])){
            return _makeError("(error) ERR value is not a valid float");
        }
        auto value=db.get(args[1]);
        if(!value.has_value()){
            value=sset();
        }
        wrong_type_operation_error=check_type(SSET,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto sset_data = std::get<sset>(value.value());
        double score_=std::stod(args[2]);
            if(sset_data.consists(args[3])){
                score_=sset_data.score(args[3]).value();
                score_=score_+std::stod(args[2]);
            };
            sset_data.put(args[3],score_);
            aof_persister.appendCommand(raw_resp_text);
            db.put(args[1],sset_data);
        return _makeSimpleString(std::to_string(score_));
    }else if (cmd=="ZCOUNT" && args.size()==4){
          if(!(is_float(args[2])&&is_float(args[3]))){
            return _makeError("(error) ERR min or max is not a float");
          }
          auto value=db.get(args[1]);
          if(!value.has_value()){
            return _makeInteger(0);
          }
          wrong_type_operation_error=check_type(SSET,value.value());
          if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
          auto sset_data=std::get<sset>(value.value());
          int count=sset_data.count(std::stof(args[2]),std::stof(args[3]));
          return _makeInteger(count);
    }else if (cmd=="ZRANGEBYSCORE" && (args.size()>=4 && args.size()<=8)){
        bool withscore=true;
        if(!(is_float(args[2])&&is_float(args[3]))){
            return _makeError("(error) ERR value not an integer or out of range.");
        }
        auto value=db.get(args[1]);
        if(!value.has_value()){
            return _makeSimpleString("(empty array)");
        }
        wrong_type_operation_error=check_type(SSET,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto sset_data=std::get<sset>(value.value());
        auto members=sset_data.range_by_score(std::stoi(args[2]),std::stoi(args[3]));
        std::vector<RESP> response_array;
        for (auto &&i : members){
            RESP item,score;
            item.type=RESP::SIMPLE_STRING;
            item.value=i.first;
            response_array.push_back(item);
            if(withscore){
                score.type=RESP::SIMPLE_STRING;
                score.value=std::to_string(i.second);
                response_array.push_back(score);
            }
        }
        return return_resp_array(response_array);
    }else if (cmd=="ZREM" && args.size()>=3){
        auto value=db.get(args[1]);
        if(!value.has_value()){
            return _makeInteger(0);
        }
        wrong_type_operation_error=check_type(SSET,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto sset_data=std::get<sset>(value.value());
        int count=0;
        for (size_t i = 2; i < args.size(); i++){
            if(sset_data.erase(args[i])){
                count++;
            }
        }
        aof_persister.appendCommand(raw_resp_text);
        db.put(args[1],sset_data);
        return _makeInteger(count);
    }
    else if(cmd=="ZSCORE" && args.size()==3){
        auto value=db.get(args[1]);
        if(!value.has_value()){
            return _makeSimpleString("(nil)");
        }
        wrong_type_operation_error=check_type(SSET,value.value());
        if(wrong_type_operation_error.has_value()){
            return wrong_type_operation_error.value();
        } 
        auto sset_data=std::get<sset>(value.value());
        auto score=sset_data.score(args[2]);
        if (!score.has_value()){
            return _makeSimpleString("(nil)");
        }
        return _makeSimpleString(std::to_string(score.value()));        
    }
    else{
        return _makeError("(error) wrong number of arguments for "+cmd+"\n"); 
    }
}