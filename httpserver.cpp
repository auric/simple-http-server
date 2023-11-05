#include "httpserver.h"

#include <iostream>
#include <regex>
#include <sstream>
#include <unistd.h>

namespace {

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
            std::cerr << "Error accepting client connection.\n";
            continue;
        }

        const auto buffSize = 1024;
        char buffer[buffSize];
        auto bytesRead = recv(m_clientSocket, buffer, buffSize, 0);
        std::istringstream request(buffer);
        std::string method, uri, protocol;
        request >> method >> uri >> protocol;

        auto it = m_handlers.find(method);
        if (it == m_handlers.end()) {
            std::cerr << "Handler for method: " << method << " not found.\n";
        }
        it->second->process(m_clientSocket, request, uri);
    }
}

void HTTPServer::appendHandler(const std::string &method, std::unique_ptr<Handler> handler)
{
    m_handlers.emplace(method, std::move(handler));
}
