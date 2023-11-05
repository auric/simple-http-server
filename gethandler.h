#pragma once

#include "handler.h"

class Database;

class GetHandler : public Handler
{
public:
    GetHandler(Database &db);

    void process(int clientSocket, std::istringstream &stream, const std::string &uri) override;

private:
    Database &m_db;
};
