#include "utils/functions.hpp"

bool is_integer(const std::string& str){
        std::regex reg {R"(^\d+$)"};
        return std::regex_match(str,reg);
}

bool is_float(const std::string& str){
        std::regex reg {R"(^\d+$)"};
        return std::regex_match(str,reg);
}
RESP return_resp_array(std::vector<RESP>& array){
        RESP response;
        response.type=RESP::ARRAY;
        response.value=array;
        return response;
}
bool is_hash_command(std::string& cmd){
        if (cmd == "HSET" || cmd == "HGET" || cmd == "HDEL"||cmd=="HGETALL"||cmd=="HMGET"||cmd=="HINCRBY"||cmd=="HEXISTS"||cmd=="HKEYS"||cmd=="HVALS"||cmd=="HLEN") return true;
        return false;
}
bool is_hll_command(std::string& cmd){
        
        return false;

}
bool is_keys_command(std::string& cmd){
        if(cmd=="EXISTS"||cmd=="EXPIRE"||cmd=="TTL"||cmd=="PERSIST"||cmd=="RENAME"||cmd=="KEYS"||cmd=="SCAN"||cmd=="DEL") return true;
        return false;
}
bool is_list_command(std::string& cmd){
        if (cmd == "LPUSH" || cmd == "RPOP" || cmd == "LRANGE"||cmd=="RPUSH"||cmd=="LPOP"||cmd=="LINDEX"||cmd=="LLEN") return true;
        return false;
}
bool is_pub_sub_command(std::string& cmd){
        if(cmd=="PUBLISH"||cmd=="SUBSCRIBE"||cmd=="UNSUBSCRIBE") return true;
        return false;
}
bool is_server_command(std::string& cmd){
 if (cmd=="PING"||cmd=="INFO"||cmd=="DBSIZE"||cmd=="FLUSHALL") return true;
        return false;
}
bool is_set_command(std::string& cmd){
         if(cmd=="SADD"||cmd=="SREM"||cmd=="SMEMBERS"||cmd=="SDIFF"||cmd=="SINTER"||cmd=="SUNION"||cmd=="SISMEMBER"||cmd=="SCARD"||cmd=="SMOVE") return true;
        return false;
}
bool is_sorted_set_command(std::string& cmd){
        if(cmd=="ZADD"||cmd=="ZRANGE"||cmd=="ZREM"||cmd=="ZSCORE"||cmd=="ZINCRBY"||cmd=="ZCARD"||cmd=="ZRANK"||cmd=="ZCOUNT"||cmd=="ZRANGEBYSCORE") return true;
        return false;
}
bool is_string_command(std::string& cmd){
if (cmd == "SET" || cmd == "GET"|| cmd=="SETEX" || cmd=="MSET" || cmd=="APPEND" || cmd=="INCR" || cmd=="DECR" || cmd=="INCRBY" || cmd=="DECRBY") return true;
        return false;
}
bool is_transactions_command(std::string& cmd){
       if (cmd=="MULTI"||cmd=="EXEC"||cmd=="DISCARD"||cmd=="WATCH") return true; 
        return false;
}

std::optional<RESP> check_type(cmd_types type,Val& data_type){
    switch (type){
    case STRING:
        if(std::holds_alternative<std::string>(data_type)) return std::nullopt;
                return _makeError("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
    case SET:
        if(std::holds_alternative<std::unordered_set<std::string>>(data_type)) return std::nullopt;
                return _makeError("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
    case LIST:
        if(std::holds_alternative<std::vector<std::string>>(data_type)) return std::nullopt;
                return _makeError("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
    case SSET:
        if(std::holds_alternative<sset>(data_type))return std::nullopt;
                return _makeError("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
    case HTABLE:
        if(std::holds_alternative<std::unordered_map<std::string,std::string>>(data_type)) return std::nullopt;
                return _makeError("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
    default:
                return _makeError("(error) WRONGTYPE Operation against a key holding the wrong kind of value");
        break;
    }
}




std::string generate_resp_text(Val& data,std::string& key){
        std::string resp_text="*";
        if (std::holds_alternative<std::string>(data)){
            auto& val=std::get<std::string>(data);
            resp_text=resp_text.append("3\r\n")
            .append("$3\r\nSET\r\n")
            .append("$").append(std::to_string(key.length())).append("\r\n")
            .append(key).append("\r\n")
            .append("$").append(std::to_string(val.length())).append("\r\n")
            .append(val).append("\r\n");
        }else if (std::holds_alternative<std::unordered_map<std::string,std::string>>(data)){
            auto& val=std::get<std::unordered_map<std::string,std::string>>(data);
            resp_text=resp_text.append(std::to_string(2+val.size()*2)).append("\r\n").append("$4\r\nHSET\r\n")
                               .append("$").append(std::to_string(key.length())).append("\r\n")
                               .append(key).append("\r\n");
            for (auto &&[key,value_] : val){
                resp_text=resp_text.append("$").append(std::to_string(key.length())).append("\r\n")
                .append(key).append("\r\n")
                .append("$").append(std::to_string(value_.length())).append("\r\n")
                .append(value_).append("\r\n");
            }
        }else if (std::holds_alternative<std::vector<std::string>>(data)){
            auto& val=std::get<std::vector<std::string>>(data);
            resp_text=resp_text.append(std::to_string(val.size()+2)).append("\r\n").append("$5\r\nLPUSH\r\n")
                               .append("$").append(std::to_string(key.length())).append("\r\n")
                               .append(key).append("\r\n");

            for (auto &&item : val){
                resp_text=resp_text.append("$").append(std::to_string(item.length())).append("\r\n")
                .append(item).append("\r\n");
            }
            
        }else if(std::holds_alternative<std::unordered_set<std::string>>(data)){
            auto& val=std::get<std::vector<std::string>>(data);
            resp_text=resp_text.append(std::to_string(val.size()+2)).append("\r\n").append("$4\r\nSADD\r\n")
                      .append("$").append(std::to_string(key.length())).append("\r\n")
                      .append(key).append("\r\n");
            for (auto &&item : val){
                resp_text=resp_text.append("$").append(std::to_string(item.length())).append("\r\n")
                .append(item).append("\r\n");
            }
            
        }else if(std::holds_alternative<sset>(data)){
             auto& val=std::get<sset>(data);
            resp_text=resp_text.append(std::to_string(2+val.member_to_score.size()*2)).append("\r\n").append("$4\r\nZADD\r\n")
                               .append("$").append(std::to_string(key.length())).append("\r\n")
                               .append(key).append("\r\n");;
            for (auto &&[key,value_] : val.member_to_score){
                resp_text=resp_text.append("$").append(std::to_string(std::to_string(value_).length())).append("\r\n")
                .append(std::to_string(value_)).append("\r\n")
                .append("$").append(std::to_string(key.length())).append("\r\n")
                .append(key).append("\r\n");
            }
        }  
        
        return resp_text;
}


void test(){
        std::cout<<"uuuuuuuuuuuuuuuu";
        return;
}