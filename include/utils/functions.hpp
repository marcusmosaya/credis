#pragma once
#include <regex>
#include <network/Resp.hpp>
#include <utils/types.hpp>
enum cmd_types {STRING,SET,SSET,HTABLE,LIST};
using Val = std::variant<
    std::string,
    std::unordered_map<std::string, std::string>,
    std::unordered_set<std::string>,
    std::vector<std::string>,
    sset
    >;
bool is_integer(const std::string& str);
bool is_float(const std::string& str);
RESP return_resp_array(std::vector<RESP>& array);
bool is_hash_command(std::string& cmd);
bool is_hll_command(std::string& cmd);
bool is_keys_command(std::string& cmd);
bool is_list_command(std::string& cmd);
bool is_pub_sub_command(std::string& cmd);
bool is_server_command(std::string& cmd);
bool is_set_command(std::string& cmd);
bool is_sorted_set_command(std::string& cmd);
bool is_string_command(std::string& cmd);
bool is_transactions_command(std::string& cmd);
std::optional<RESP> check_type(cmd_types type,Val& data_type);

std::string generate_resp_text(Val& data,std::string& key);