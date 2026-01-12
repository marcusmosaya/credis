#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>
#include <map>

// ----- RESP Value Definition -----

struct RESP;
using RESPValue = std::variant<
    std::string,              // Simple String / Error / Bulk String
    int64_t,                  // Integer
    std::vector<RESP>         // Array
>;

struct RESP {
    enum Type { SIMPLE_STRING, ERROR, INTEGER, BULK_STRING, ARRAY } type;
    RESPValue value;
};

class RESPParser {
public:
    explicit RESPParser(const char* input);
    RESPParser();
    RESP parse();
    void setInput(const char* input);
private:
    std::stringstream in;

    std::string readLine();

    RESP parseSimpleString(RESP::Type type);

    RESP parseInteger();

    RESP parseBulkString();

    RESP parseArray();
};

class RESPGenerator {
public:
    static std::string encode(const RESP &r);
};
inline RESP _makeSimpleString(const std::string &s) { return { RESP::SIMPLE_STRING, s }; }
inline RESP _makeError(const std::string &s) { return { RESP::ERROR, s }; }
inline RESP _makeBulkString(const std::string &s) { return { RESP::BULK_STRING, s }; }
inline RESP _makeInteger(int64_t i) { return { RESP::INTEGER, i }; }