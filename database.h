#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

struct Data
{
    std::string contentType;
    std::string data;
};

class Database
{
public:
    void append(const std::string &uri, const Data &data);
    std::optional<Data> get(const std::string &uri, const std::optional<int> version) const;

    void removeLast(const std::string &uri);

private:
    std::unordered_map<std::string, std::vector<Data>> m_dataMap;
};
