#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <unordered_map>

class HTTPServer {
public:
    HTTPServer();

    void run();

private:
    void parsePost(std::istringstream &request, const std::string &uri);
    void handleGet(const std::string& uri);
    void handlePost(const std::string& uri, const std::string& body, const std::string& contentType);
    void handleDelete(const std::string& uri);

private:
    int m_serverSocket;
    int m_clientSocket;
    struct sockaddr_in m_serverAddr;
    struct sockaddr_in m_clientAddr;
    socklen_t m_clientAddrLen;
    std::unordered_map<std::string, std::pair<std::string, std::string>> m_dataMap;

};

