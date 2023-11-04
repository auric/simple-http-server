#include "httpserver.h"

#include <iostream>
#include <cstring>
#include <regex>
#include <sstream>
#include <unistd.h>

namespace {

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }

    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }

    result.push_back(str.substr(start));
    return result;
}

template<class Resource>
class ScopedResource
{
public:
    using Deleter = std::function<void(Resource)>;
    ScopedResource(Resource resource, const Deleter &deleter)
        : m_resource(resource)
        , m_deleter(deleter)
    {}

    auto get() const { return m_resource; }

    ~ScopedResource()
    {
        if (m_resource) {
            m_deleter(m_resource);
        }
    }

private:
    Resource m_resource;
    Deleter m_deleter;
};
}

HTTPServer::HTTPServer() {
    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSocket < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return;
    }

    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_port = htons(8000);
    m_serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_serverSocket, (struct sockaddr *)&m_serverAddr, sizeof(m_serverAddr)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
        return;
    }

    listen(m_serverSocket, 5);
    m_clientAddrLen = sizeof(m_clientAddr);
}

void HTTPServer::run()
{
    while (true) {
        m_clientSocket = accept(m_serverSocket, (struct sockaddr *) &m_clientAddr, &m_clientAddrLen);
        ScopedResource<int> clientResource(m_clientSocket, &close);
        if (m_clientSocket < 0) {
            std::cerr << "Error accepting client connection" << std::endl;
            continue;
        }

        const auto buffSize = 1024;
        char buffer[buffSize];
        auto bytesRead = recv(m_clientSocket, buffer, buffSize, 0);
        std::istringstream request(buffer);
        std::string method, uri, protocol;
        request >> method >> uri >> protocol;

        if (method == "GET") {
            handleGet(uri);
        } else if (method == "POST") {
            parsePost(request, uri);
        } else if (method == "DELETE") {
            handleDelete(uri);
        }
    }
}

void HTTPServer::parsePost(std::istringstream &request, const std::string &uri)
{
    std::string contentTypeVerb = "Content-Type: ";
    std::string contentLengthVerb = "Content-Length: ";
    int contentLength = 0;
    std::string contentType = "application/octet-stream";
    std::string content;
    std::string emptyLine;
    std::getline(request, emptyLine);
    while (request) {
        std::string line;
        std::getline(request, line);
        if (line.find(contentTypeVerb) == 0) {
            contentType = trim(line.substr(contentTypeVerb.size()));
        } else if (line.find(contentLengthVerb) == 0) {
            const auto contentLengthStr = trim(line.substr(contentLengthVerb.size()));
            contentLength = std::stoi(contentLengthStr);
        } else if (line == "\r") {
            break;
        }
    }

    std::getline(request, content, '\0');
    if (contentLength > content.size()) {
        const auto buffSize = 1024;
        char buffer[buffSize];
        contentLength -= content.size();
        while (contentLength > 0) {
            auto bytesRead = recv(m_clientSocket, buffer, buffSize, 0);
            contentLength -= bytesRead;
            content.append(buffer, bytesRead);
        }
    }
    handlePost(uri, content, contentType);
}

void HTTPServer::handleGet(const std::string& uri) {
    std::string response;
    if (m_dataMap.count(uri) == 0) {
        response = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
        send(m_clientSocket, response.c_str(), response.size(), 0);
    } else {
        std::string data = m_dataMap[uri].first;
        std::string contentType = m_dataMap[uri].second;

        // Calculate the content length
        int contentLength = data.length();

        // Construct the response with Content-Length and Connection: close headers
        response = "HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\nContent-Length: " + std::to_string(contentLength) + "\r\nConnection: close\r\n\r\n" + data;

        // Send the response
        send(m_clientSocket, response.c_str(), response.size(), 0);
    }
}


void HTTPServer::handlePost(const std::string& uri, const std::string& body, const std::string& contentType) {
    m_dataMap[uri] = std::make_pair(body, contentType);
    std::string response = "HTTP/1.1 200 OK\r\n\r\nData stored successfully.";
    send(m_clientSocket, response.c_str(), response.size(), 0);
}

void HTTPServer::handleDelete(const std::string& uri) {
    if (m_dataMap.count(uri) == 0) {
        std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(m_clientSocket, response.c_str(), response.size(), 0);
    } else {
        m_dataMap.erase(uri);
        std::string response = "HTTP/1.1 200 OK\r\n\r\nData deleted successfully.";
        send(m_clientSocket, response.c_str(), response.size(), 0);
    }
}
