#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <memory>
#include <string>
#include <unordered_map>

#include "handler.h"

class HTTPServer {
public:
    HTTPServer();

    void run();
    void appendHandler(const std::string &method, std::unique_ptr<Handler> handler);

private:
    int m_serverSocket;
    int m_clientSocket;
    struct sockaddr_in m_serverAddr;
    struct sockaddr_in m_clientAddr;
    socklen_t m_clientAddrLen;
    std::unordered_map<std::string, std::unique_ptr<Handler>> m_handlers;
};

