#include "deletehandler.h"

#include <sys/socket.h>

#include "database.h"

DeleteHandler::DeleteHandler(Database &db)
    : m_db(db)
{}

void DeleteHandler::process(int clientSocket, std::istringstream &stream, const std::string &uri)
{
    auto data = m_db.get(uri);
    if (!data) {
        std::string response = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(clientSocket, response.c_str(), response.size(), 0);
    } else {
        m_db.remove(uri);
        std::string response = "HTTP/1.1 200 OK\r\n\r\nData deleted successfully.";
        send(clientSocket, response.c_str(), response.size(), 0);
    }
}
