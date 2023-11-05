#include <iostream>

#include "database.h"
#include "deletehandler.h"
#include "gethandler.h"
#include "httpserver.h"
#include "posthandler.h"

int main()
{
    Database db;

    HTTPServer server;
    server.appendHandler("GET", std::make_unique<GetHandler>(db));
    server.appendHandler("POST", std::make_unique<PostHandler>(db));
    server.appendHandler("DELETE", std::make_unique<DeleteHandler>(db));
    server.run();
    return 0;
}
