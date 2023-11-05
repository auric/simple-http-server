#include "gethandler.h"

#include <sys/socket.h>

#include "database.h"

GetHandler::GetHandler(Database &db)
    : m_db(db)
{
}

void GetHandler::process(int clientSocket, std::istringstream &stream, const std::string &uri)
{
    std::string response;
    auto res = m_db.get(uri);
    if (!res) {
        response = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
        send(clientSocket, response.c_str(), response.size(), 0);
    } else {
        const std::string &data = res->data;
        const std::string &contentType = res->contentType;

        // Calculate the content length
        int contentLength = data.length();

        // Construct the response with Content-Length and Connection: close headers
        response = "HTTP/1.1 200 OK\r\nContent-Type: " + contentType + "\r\nContent-Length: "
                   + std::to_string(contentLength) + "\r\nConnection: close\r\n\r\n" + data;

        // Send the response
        send(clientSocket, response.c_str(), response.size(), 0);
    }
}
