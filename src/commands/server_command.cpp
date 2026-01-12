#include "commands/server_command.hpp"

RESP ServerCommand::execute(const std::string& raw_resp_text,const std::vector<std::string>& args,Memory_Store& db,AOF_Persist& aof_persister,Expire_Manager& exp) {
    if (args.empty()){
        std::string err="empty command";
        return _makeError(err);
    } 
    const std::string& cmd = args[0];
    if((cmd=="PING"&& args.size()==1)|| (cmd=="PING" && args.size()==2)){
        if (args.size()==1)
        {
            return _makeSimpleString("PONG");
        }
      return  _makeSimpleString(args[1]);
        
    }else if ((cmd=="INFO" && args.size()==1 )|| (cmd=="INFO" && args.size()==2))
    {   
        std::unordered_map<std::string,std::string> x={{"version","a.a.a"},{"clients","50"},{"server_name","credis1"}};
        if(args.size()==1){
            std::vector<RESP> response_array;
            for (auto &&[k,v] : x)
            {   
                RESP temp;
                temp.type=RESP::SIMPLE_STRING;
                temp.value=k+":"+v;
                response_array.emplace_back(temp);
            }
            return return_resp_array(response_array);
        }else
        {
            return _makeError("(error) Under development.");
        }
        
    }else if (cmd=="DBSIZE" && args.size()==1)
    {
        return _makeInteger(db.size());
        /* code */
    }else if ((cmd=="FLUSHALL" && args.size()==1)|| (cmd=="FLUSHALL" && args.size()==2))
    {   
        if (args.size()==1){
            return _makeError("(error) under development");
        }else if (args[1]=="ASYNC"){
            db.clear();
            return _makeSimpleString("OK");
        }else
        {
            return _makeError("(error) illegal arguement");
        }
    }else if (cmd=="SELECT" && args.size()==2)
    {
        /* code */
        return _makeError("(error) under development");
    }else{
        return _makeError("(error) wrong number of arguments for "+cmd+"\n");   
    }
    
    
    
    
}