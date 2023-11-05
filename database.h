#pragma once

#include <optional>
#include <string>
#include <unordered_map>

struct Data
{
    std::string contentType;
    std::string data;
};

class Database
{
public:
    Database();

    void append(const std::string &uri, const Data &data);
    std::optional<Data> get(const std::string &uri) const;

    void remove(const std::string &uri);

private:
    std::unordered_map<std::string, Data> m_dataMap;
};
