// Client side C/C++ program to demonstrate Socket programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>   // for errno
#include <limits.h>  // for INT_MAX, INT_MIN
#include <stdlib.h>  // for strtol

#include "common.h"

int main(int argc, char const *argv[]) {
    const char *pseudo = argv[1];
    const char *ip = argv[2]; // 127.0.0.1
    const char *temp_port = argv[3];
    // converting char to int
    int port = conv_port(temp_port);

    int sock = checked(socket(AF_INET, SOCK_STREAM, 0));
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Conversion de string vers IPv4 ou IPv6 en binaire
    checked(inet_pton(AF_INET, ip, &serv_addr.sin_addr));

    checked(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)));
    char buffer[1024];
    ssize_t nbytes = 1;
    while (nbytes > 0 && fgets(buffer, 1024, stdin)) {
        // Supprimer le \n
        size_t len = strlen(buffer);
        buffer[len - 1] = '\0';
        // On garde la même taille de string pour explicitement envoyer le '\0'
        nbytes = ssend(sock, buffer, len);
        if (nbytes > 0) {
            char *recvbuffer;
            nbytes = receive(sock, (void *)&recvbuffer);
            if (nbytes > 0) {
                printf("Echo: %s\n", recvbuffer);
                free(recvbuffer);
            }
        }
    }
    return 0;
}
