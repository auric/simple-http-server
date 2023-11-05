#pragma once

#include <sstream>
#include <string>

#include "handler.h"

class Database;

class PostHandler : public Handler
{
public:
    PostHandler(Database &db);

    void process(int clientSocket, std::istringstream &request, const std::string &uri) override;

private:
    void saveAndSendReply(int clientSocket,
                          const std::string &uri,
                          const std::string &body,
                          const std::string &contentType);

private:
    Database &m_db;
};
