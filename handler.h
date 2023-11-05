#pragma once

#include <sstream>
#include <string>

struct Handler
{
    virtual ~Handler() = default;
    virtual void process(int clientSocket, std::istringstream &request, const std::string &uri) = 0;
};
