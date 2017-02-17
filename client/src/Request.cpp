#include "Request.h"

std::string Request::getBody() {
    return body;
}

Request::Request(std::string body) {
    this->body = body;
}
