#pragma once
#include <variant>
#include <unordered_map>
#include <string>
#include <vector>
#include <list>
#include <unordered_set>
#include <utils/sset.hpp>
#include <regex>
#include <iostream>
#include <sstream>
#include <chrono>


using Value = std::variant<
    std::string,
    std::unordered_map<std::string, std::string>,
    std::unordered_set<std::string>,
    std::vector<std::string>,
    sset
    >;

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;



struct Node {
   std::string key; 
   Value value;
   int freq;
   };
