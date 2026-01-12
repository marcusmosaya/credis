#include "network/Resp.hpp"

RESPParser::RESPParser(const char * input){
    in<<input;
}

void RESPParser::setInput(const char * input){
    in<<input;
}
RESP RESPParser::parse() {
        char prefix;
        if (!in.get(prefix)) throw std::runtime_error("Unexpected end of input");
        switch (prefix) {
            case '+': return parseSimpleString(RESP::SIMPLE_STRING);
            case '-': return parseSimpleString(RESP::ERROR);
            case ':': return parseInteger();
            case '$': return parseBulkString();
            case '*': return parseArray();
            default: throw std::runtime_error("Invalid RESP type prefix");
        }
    }
std::string RESPParser::readLine() {
        std::string line;
        std::getline(in, line);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        return line;
    }

RESP RESPParser::parseSimpleString(RESP::Type type) {
        RESP r;
        r.type = type;
        r.value = readLine();
        return r;
    }

RESP RESPParser::parseInteger() {
        RESP r;
        r.type = RESP::INTEGER;
        std::string line = readLine();
        r.value = std::stoll(line);
        return r;
    }

RESP RESPParser::parseBulkString() {
        int64_t len;
        in >> len;
        in.ignore(2); // skip CRLF
        RESP r;
        r.type = RESP::BULK_STRING;
        if (len == -1) {
            r.value = std::string();
        } else {
            std::string data(len, '\0');
            in.read(&data[0], len);
            in.ignore(2); // CRLF
            r.value = data;
        }
        return r;
    }

RESP RESPParser::parseArray() {
        int64_t count;
        in >> count;
        in.ignore(2); // skip CRLF
        RESP r;
        r.type = RESP::ARRAY;
        std::vector<RESP> items;
        for (int i = 0; i < count; ++i)
            items.push_back(parse());
        r.value = std::move(items);
        return r;
    }

std::string RESPGenerator::encode(const RESP &r) {
        std::ostringstream out;
        switch (r.type) {
            case RESP::SIMPLE_STRING:
                out << "+" << std::get<std::string>(r.value) << "\r\n";
                break;
            case RESP::ERROR:
                out << "-" << std::get<std::string>(r.value) << "\r\n";
                break;
            case RESP::INTEGER:
                out << ":" << std::get<int64_t>(r.value) << "\r\n";
                break;
            case RESP::BULK_STRING: {
                auto &s = std::get<std::string>(r.value);
                out << "$" << s.size() << "\r\n" << s << "\r\n";
                break;
            }
            case RESP::ARRAY: {
                auto &arr = std::get<std::vector<RESP>>(r.value);
                out << "*" << arr.size() << "\r\n";
                for (auto &item : arr) out << encode(item);
                break;
            }
        }
        return out.str();
    }
