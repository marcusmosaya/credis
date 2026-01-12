#pragma once
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <optional>
struct sset{
        std::unordered_map<std::string, double> member_to_score;
        std::multimap<double, std::string> score_to_member;
        bool consists(const std::string& member)const;
        bool put(const std::string& member,double score);
        bool erase(const std::string& member);
        double get(const std::string& member);
        std::vector<std::pair<std::string,double>> range(size_t start,size_t top);
        std::vector<std::pair<std::string,double>> range_by_score(double min_score,double max_score) const;
        std::optional<double>  score(const std::string& member);
        std::optional<std::vector<std::pair<int,double>>> rank(const std::string& member);
        int cardinality() const;
        int count(double min_score,double max_score) const;
        double increase_by(const std::string& member,double increment);
};