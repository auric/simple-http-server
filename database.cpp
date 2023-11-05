#include "database.h"

Database::Database()
{
}

void Database::append(const std::string &uri, const Data &data)
{
    m_dataMap.insert_or_assign(uri, data);
}

std::optional<Data> Database::get(const std::string &uri) const
{
    auto it = m_dataMap.find(uri);
    if (it != m_dataMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

void Database::remove(const std::string &uri)
{
    m_dataMap.erase(uri);
}
