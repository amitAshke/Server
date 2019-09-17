#ifndef SERVER_CLIENTHANDLER_H
#define SERVER_CLIENTHANDLER_H

#include <string>

using namespace std;

template <class P, class S>
class ClientHandler {
public:
    virtual void handleClient(int socket) = 0;
};

#endif //SERVER_CLIENTHANDLER_H
