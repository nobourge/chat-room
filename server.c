// Server side C/C++ program to demonstrate Socket programming

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>   // for errno
#include <limits.h>  // for INT_MAX, INT_MIN
#include <stdlib.h>  // for strtol
#include <stdbool.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>  //close

#include "common.h"

void *echo(void *socket_ptr) {
    printf("Client is connected\n");
    int client_socket = *((int *)socket_ptr);
    ssize_t nbytes = 1;
    while (nbytes > 0) {
        char *buffer;
        nbytes = receive(client_socket, (void **)&buffer);
        printf("Received %ld bytes: %s\n", nbytes, buffer);
        if (nbytes > 0) {
            nbytes = ssend(client_socket, buffer, nbytes);
            free(buffer);
        }
    }
    printf("Client disconnected\n");
    return NULL;
}

int main(int argc, char const *argv[]) {
    // Creating socket file descriptor
//    int server_fd = checked(socket(AF_INET, SOCK_STREAM, 0));

    const char *temp_port = argv[1];
    // converting char to int
    int port = conv_port(temp_port);

    int opt = 1;
    int master_socket = checked(socket(AF_INET, SOCK_STREAM, 0));
    checked(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)));

    // type of socket created
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    checked(bind(master_socket, (struct sockaddr *) &address, sizeof(address)));
    checked(listen(master_socket, 3));

    size_t addrlen = sizeof(address);
    int clients[1024];
    pthread_t threads[1024];
    int nclients = 0;
    while (true) {
        clients[nclients] = checked(accept(master_socket, (struct sockaddr *) &address, (socklen_t * ) & addrlen));
        if (pthread_create(&threads[nclients], NULL, echo, &clients[nclients]) != 0) {
            perror("Could not create the thread\n");
            exit(1);
        }
        nclients++;
    }
}
//    int opt = 1;
//    int master_socket = checked(socket(AF_INET, SOCK_STREAM, 0));
//    checked(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)));
//    // type of socket created
//    struct sockaddr_in address;
//    address.sin_family = AF_INET;
//    address.sin_addr.s_addr = INADDR_ANY;
//    address.sin_port = htons(8080);
//    checked(bind(master_socket, (struct sockaddr *) &address, sizeof(address)));
//    checked(listen(master_socket, 3));
//
//    socklen_t addrlen = sizeof(address);
//    fd_set readfds;
//    int clients[1024];
//    int nclients = 0;
//    int max_fd = master_socket;
//    while (true) {
//        FD_ZERO(&readfds);
//        FD_SET(master_socket, &readfds);
//        for (int i = 0; i < nclients; i++) {
//            FD_SET(clients[i], &readfds);
//        }
//        checked(select(max_fd + 1, &readfds, NULL, NULL, NULL));
//
//        // Nouvelles connexion
//        if (FD_ISSET(master_socket, &readfds)) {
//            int nouveau_socket = checked(accept(master_socket, (struct sockaddr *) &address, &addrlen));
//            clients[nclients] = nouveau_socket;
//            nclients++;
//            max_fd = (nouveau_socket > max_fd) ? nouveau_socket : max_fd;
//        }
//            // Echo
//        else {
//            for (int i = 0; i < nclients; i++) {
//                if (FD_ISSET(clients[i], &readfds)) {
//                    char buffer[1024];
//                    ssize_t nbytes = checked(read(clients[i], buffer, 1024));
//                    if (nbytes == 0) {
//                        printf("Le client est deconnecte\n");
//                        close(clients[i]);
//                        nclients--;
//                        clients[i] = clients[nclients];
//                        if (max_fd == clients[i]) {
//                            max_fd = 0;
//                            for (int j = 0; j < nclients; j++) {
//                                max_fd = (clients[j] > max_fd) ? clients[j] : max_fd;
//                            }
//                        }
//                    } else {
//                        printf("Recu: %s\n", buffer);
//                        checked(write(clients[i], buffer, nbytes));
//                    }
//                }
//            }
//        }
//
//        return 0;
//    }
//}