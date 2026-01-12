#include "utils/sset.hpp"
#include <iostream>
bool sset::put(const std::string& member,double score ){
        // Check if member already exists
        auto it = member_to_score.find(member);
        if (it != member_to_score.end()) {
            // Member exists, need to update
            double old_score = it->second;
            if (old_score == score) return false; // No change
            
            // Remove old score->member mapping
            auto range = score_to_member.equal_range(old_score);
            for (auto i = range.first; i != range.second; ++i) {
                if (i->second == member) {
                    score_to_member.erase(i);
                    break;
                }
            }
        }
        
        // Add/update the mappings
        member_to_score[member] = score;
        score_to_member.insert({score, member});
        return true;
}

bool sset::erase(const std::string& member){
        auto it = member_to_score.find(member);
        if (it == member_to_score.end()) return false;
        
        double score = it->second;
        member_to_score.erase(it);
        
        // Remove from sorted structure
        auto range = score_to_member.equal_range(score);
        for (auto i = range.first; i != range.second; ++i) {
            if (i->second == member) {
                score_to_member.erase(i);
                break;
            }
        }
        return true;
}

std::vector<std::pair<std::string,double>> sset::range(size_t start,size_t stop) {
        std::vector<std::pair<std::string, double>> result;
        
        if (start > stop || start >= score_to_member.size()) {
            return result;
        }
        stop = std::min(stop,score_to_member.size());
        auto it = score_to_member.begin();
        std::advance(it, start);
        
        for (size_t i = start; i <= stop && it != score_to_member.end(); ++i, ++it) {
            result.push_back({it->second, it->first});
        }
        return result;

}
std::vector<std::pair<std::string,double>> sset::range_by_score(double min_score,double max_score) const{
        std::vector<std::pair<std::string, double>> result;
        auto start = score_to_member.lower_bound(min_score);
        auto end = score_to_member.upper_bound(max_score);
        for (auto it = start; it != end; ++it) {
            result.push_back({it->second, it->first});
        }
        return result;
}
std::optional<double>  sset::score(const std::string& member){
         auto it = member_to_score.find(member);
        if (it == member_to_score.end()) return std::nullopt;
        return it->second;

}
 std::optional<std::vector<std::pair<int,double>>> sset::rank(const std::string& member){
        auto it = member_to_score.find(member);
        if (it == member_to_score.end()) return std::nullopt;
        
        double score = it->second;
        size_t rank = 0;
        std::vector<std::pair<int,double>> response;
        for (auto& pair : score_to_member) {
            if (pair.first > score) break;
            if (pair.first == score && pair.second == member) break;
            rank++;
        }
        response={{rank,score}};
        return response;
    
}
int sset::cardinality() const{
                return member_to_score.size();

}
int sset::count(double min_score,double max_score) const{
        auto start = score_to_member.lower_bound(min_score);
        auto end = score_to_member.upper_bound(max_score);
        return std::distance(start, end);
}
double sset::increase_by(const std::string& member,double increment){
        double new_score = increment;
        auto it = member_to_score.find(member);
        if (it != member_to_score.end()) {
            new_score += it->second;
        }
        put(member, new_score);
        return new_score;
}

bool sset::consists(const std::string& member)const{
        if(member_to_score.count(member))return true;
        return false;
        
}

double sset::get(const std::string& member){
        auto it = member_to_score.find(member);
        return it->second;
}