#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>  //close

#include "common.h"

int main(int argc, char *argv[]) {
    int opt = 1;
    int master_socket = checked(socket(AF_INET, SOCK_STREAM, 0));
    checked(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)));

    // type of socket created
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    checked(bind(master_socket, (struct sockaddr *)&address, sizeof(address)));
    checked(listen(master_socket, 3));

    socklen_t addrlen = sizeof(address);
    fd_set readfds;
    int clients[1024];
    int nclients = 0;
    int max_fd = master_socket;
    while (true) {
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        for (int i = 0; i < nclients; i++) {
            FD_SET(clients[i], &readfds);
        }
        checked(select(max_fd + 1, &readfds, NULL, NULL, NULL));

        // Nouvelles connexion
        if (FD_ISSET(master_socket, &readfds)) {
            int nouveau_socket = checked(accept(master_socket, (struct sockaddr *)&address, &addrlen));
            clients[nclients] = nouveau_socket;
            nclients++;
            max_fd = (nouveau_socket > max_fd) ? nouveau_socket : max_fd;
        }
            // Echo
        else {
            for (int i = 0; i < nclients; i++) {
                if (FD_ISSET(clients[i], &readfds)) {
                    char buffer[1024];
                    ssize_t nbytes = checked(read(clients[i], buffer, 1024));
                    if (nbytes == 0) {
                        printf("Le client est deconnecte\n");
                        close(clients[i]);
                        nclients--;
                        clients[i] = clients[nclients];
                        if (max_fd == clients[i]) {
                            max_fd = 0;
                            for (int j = 0; j < nclients; j++) {
                                max_fd = (clients[j] > max_fd) ? clients[j] : max_fd;
                            }
                        }
                    } else {
                        printf("Recu: %s\n", buffer);
                        checked(write(clients[i], buffer, nbytes));
                    }
                }
            }
        }
    }
    return 0;
}