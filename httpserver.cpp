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
        if (m_clientSocket < 0) {
            std::cerr << "Error accepting client connection" << std::endl;
            continue;
        }

        char buffer[1024];
        recv(m_clientSocket, buffer, sizeof(buffer), 0);
        std::istringstream request(buffer);
        std::string method, uri, protocol;
        request >> method >> uri >> protocol;

        std::string contentTypeVerb = "Content-Type: ";
        std::string contentLengthVerb = "Content-Length: ";
        if (method == "GET") {
            handleGet(uri);
        } else if (method == "POST") {
            std::string contentType = "application/octet-stream";
            std::string tmp;
            std::string content;
            while (request) {
                std::getline(request, tmp);
                auto contentTypePos = tmp.find(contentTypeVerb);
                if (contentTypePos == 0) {
                    contentType = trim(tmp.substr(contentTypeVerb.size()));
                } else if (tmp.find(contentLengthVerb) == 0) {
                    while (request.get() != '\n');
                    std::getline(request, content);
                    break;
                }
            }
            handlePost(uri, content, contentType);
        } else if (method == "DELETE") {
            handleDelete(uri);
        }

        close(m_clientSocket);
    }
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
