#pragma once

#include "handler.h"

class Database;

class DeleteHandler : public Handler
{
public:
    DeleteHandler(Database &db);

    void process(int clientSocket, std::istringstream &stream, const std::string &uri) override;

private:
    Database &m_db;
};
