#include "ClientHandler.h"

template <class P, class S>
class OpenServerArgs {
    ClientHandler<P,S> *clientHandler;
    int port;
    bool* shouldStop;
public:
    OpenServerArgs(int port1, ClientHandler<P,S> *clientHandler1, bool *shouldStop1) {
        clientHandler = clientHandler1;
        port = port1;
        shouldStop = shouldStop1;
    }
    int getPort() { return port; }
    ClientHandler<P,S>* getClientHandler() { return clientHandler; }
    bool *getShouldStop() { return shouldStop; }
};
