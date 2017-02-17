#ifndef FILESHARER_REQUEST_H
#define FILESHARER_REQUEST_H

#include <string>

class Request {
    private:
        std::string body;

    public:
        Request(std::string body);
        virtual void process() = 0;
        std::string getBody();
};


#endif //FILESHARER_REQUEST_H
