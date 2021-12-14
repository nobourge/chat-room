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

const int BUFF_SIZE = 1024;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("USAGE: $ ./server <port> \n %d", argc);
        exit(1);
    }
    const char *temp_port = argv[1];
    // converting char to int
    int port = conv_port(temp_port);

    int opt = 1;
    int master_socket = checked(socket(AF_INET, SOCK_STREAM, 0));
    checked(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)));

    // type of socket created
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    checked(bind(master_socket, (struct sockaddr *)&address, sizeof(address)));
    checked(listen(master_socket, 3));

    size_t addrlen = sizeof(address);
    fd_set readfds;
    int clients[BUFF_SIZE];
    int nclients = 0;

    //database indice=id value=pseudo (a voir si ca marche)
    //double users[1024];
    const char *users[8];

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        int max_fd = master_socket;
        for (int i = 0; i < nclients; i++) {
            FD_SET(clients[i], &readfds);
            if (clients[i] > max_fd) {
                max_fd = clients[i];
            }
        }
        // wait for an activity on one of the sockets, timeout is NULL
        select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(master_socket, &readfds)) {
            // Si c'est le master socket qui a des donnees, c'est une nouvelle connexion.
            clients[nclients] = accept(master_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);
            nclients++;

//            for (int i = 0; i < nclients; i++) {
//                //char *pseudo;
//                char *pseudo;
//
//                //size_t nbytez = receive(clients[i], (void *) &pseudo);
//                size_t nbytez = receive(clients[i], (void *) &pseudo);
//                //users[clients[i]] = *pseudo;
//                users[clients[i]] = &pseudo;
//
//                //a[0] = "blah";
//                //a[1] = "hmm";
//                printf("pseudo = %d", *pseudo);
//            }

        }
        else
        {
            // Sinon, c'est un message d'un client
            for (int i = 0; i < nclients; i++) {
                if (FD_ISSET(clients[i], &readfds)) {
                    char *buffer;
                    size_t nbytes = receive(clients[i], (void *)&buffer);
                    if (nbytes > 0) {  // closed
                        // printf("User %s a dit %s\n", users[clients[i]], buffer);
                        printf("User %d a dit %s\n", clients[i], buffer);
                        for (int j = 0; j < nclients; j++){
                            ssend(clients[j], buffer, nbytes);
                        }
                        free(buffer);
                    }
                    else {
                        close(clients[i]);
                        // On deplace le dernier socket a la place de libre pour ne pas faire de trou.
                        clients[i] = clients[nclients - 1];
                        nclients--;
                    }
                }
            }
        }
    }
    return 0;
}