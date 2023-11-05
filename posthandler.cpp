#include "posthandler.h"

#include <sys/socket.h>

#include "database.h"
#include "utils.h"

PostHandler::PostHandler(Database &db)
    : m_db(db)
{}

void PostHandler::process(int clientSocket, std::istringstream &request, const std::string &uri)
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
            contentType = utils::trim(line.substr(contentTypeVerb.size()));
        } else if (line.find(contentLengthVerb) == 0) {
            const auto contentLengthStr = utils::trim(line.substr(contentLengthVerb.size()));
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
            auto bytesRead = recv(clientSocket, buffer, buffSize, 0);
            contentLength -= bytesRead;
            content.append(buffer, bytesRead);
        }
    }
    saveAndSendReply(clientSocket, uri, content, contentType);
}

void PostHandler::saveAndSendReply(int clientSocket,
                                   const std::string &uri,
                                   const std::string &body,
                                   const std::string &contentType)
{
    m_db.append(uri, {contentType, body});
    std::string response = "HTTP/1.1 200 OK\r\n\r\nData stored successfully.";
    send(clientSocket, response.c_str(), response.size(), 0);
}
