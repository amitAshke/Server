#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <thread>

#include "Server.h"
#include "ClientHandler.h"
#include "OpenServerArgs.h"
#include "ParallelClientArgs.h"

template <class P, class S>
class MyParallelServer : public Server<P,S> {
    vector<pthread_t> threads;
    bool shouldStop = false;
public:
    void open(int port, ClientHandler<P,S>* clientHandler) {
        auto commArgs = new OpenServerArgs<P,S>(port, clientHandler, &shouldStop);

        pthread_t pthread;
        pthread_create(&pthread, nullptr, MyParallelServer::openServerThread, (void*)(commArgs));
        threads.push_back(pthread);
        //this_thread::sleep_for(chrono::milliseconds(100000));
    }

    static void* openServerThread(void* commArgs) {
        int port = ((OpenServerArgs<P,S>*) commArgs)->getPort();
        ClientHandler<P,S> *clientHandler = ((OpenServerArgs<P,S>*) commArgs)->getClientHandler();
        bool* shouldStop = ((OpenServerArgs<P,S>*) commArgs)->getShouldStop();

        int sockfd, newsockfd, clilen;
        struct sockaddr_in serv_addr, cli_addr;

        vector<pthread_t> localThreads;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        /* Check of creation succeeded*/
        if (sockfd < 0) {
            perror("Failed to open connection.");
            exit(1);
        }

        /* Initialize m_socket structure */
        bzero((char *) &serv_addr, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons((uint16_t)((size_t)port));

        /* Now bind the host address using bind() call.*/
        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("Failed to bind server connection.");
            exit(1);
        }

        listen(sockfd,10);
        clilen = sizeof(cli_addr);

        while (true) {

            newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t*)&clilen);

            if (newsockfd < 0) {
                if (*shouldStop) {
                    break;
                }
                if (errno == EWOULDBLOCK) {
                    continue;
                }
                perror("ERROR on accept");
                exit(1);
            }

            auto commArgs = new ParallelClientArgs<P,S>(newsockfd, clientHandler);

            pthread_t pthread;
            pthread_create(&pthread, nullptr, MyParallelServer::singleClientCom, (void*)(commArgs));
            localThreads.push_back(pthread);

            timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
        }

        for (int i = 0; i < localThreads.size(); i++) {
            pthread_join(localThreads[i], nullptr);
        }
    }

    static void* singleClientCom(void* commArgs) {
        int socketfd = ((ParallelClientArgs<P,S>*) commArgs)->getSocket();
        ClientHandler<P,S>* clientHandler = ((ParallelClientArgs<P,S>*) commArgs)->getClientHandler();

        clientHandler->handleClient(socketfd);
    }

    void stop() {
        shouldStop = true;
        for (int i = 0; i < threads.size(); i++) {
            pthread_join(threads[i], nullptr);
        }
    }

};
