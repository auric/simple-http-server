#include "database.h"

#include <algorithm>

void Database::append(const std::string &uri, const Data &data)
{
    auto it = m_dataMap.find(uri);
    if (it != m_dataMap.end()) {
        it->second.push_back(data);
    } else {
        m_dataMap.emplace(uri, std::vector<Data>{data});
    }
}

std::optional<Data> Database::get(const std::string &uri, const std::optional<int> version) const
{
    auto it = m_dataMap.find(uri);
    if (it != m_dataMap.end()) {
        const auto &values = it->second;
        const auto last = values.size() - 1;
        size_t v = last;
        if (version) {
            v = *version - 1;
        }
        if (v < values.size()) {
            return it->second[v];
        }
    }
    return std::nullopt;
}

void Database::removeLast(const std::string &uri)
{
    auto it = m_dataMap.find(uri);
    if (it != m_dataMap.end()) {
        it->second.pop_back();
    }
}
