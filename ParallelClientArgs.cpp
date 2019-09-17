#include "ClientHandler.h"

template <class P, class S>
class ParallelClientArgs {
    int socket;
    ClientHandler<P,S> *clientHandler;
public:
    ParallelClientArgs(int socket1, ClientHandler<P,S> *clientHandler1) {
        clientHandler = clientHandler1;
        socket = socket1;
    }
    int getSocket() { return socket; }
    ClientHandler<P,S>* getClientHandler() { return clientHandler; }
};
