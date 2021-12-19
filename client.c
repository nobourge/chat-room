/* Ce qu'il reste a faire :
 *
 * Client : L’utilisateur souhaite se déconnecter, et appuie sur CTRL+D -> boucle infinie bug !
 * Client : Si l connexion est perdue avec le serveur on affiche un message à l’utilisateur -> dunno?
 * Client : N’oubliez pas de gérer les accès concurrents aux ressources partagées -> semaphores?
 * Server : select -> a reviser !
 * PDF rapport !
 * Code : commentaires + docstrings !
 * !!! Le message doit comprendre : 1. size 2. timestamp 3. message -> on le fait a la fin
 * !!! Pour l'instant on envoie le timestamp et le message separement.
 */

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
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>

#include "common.h"

void* freceive(void *socket) {
    int sock = *(int*)socket;
    ssize_t nbytes = 1;

    while (nbytes != 0) {
        char *recvbuffer;
        char *recvtimestamp;
        char *pseudo;
        ssize_t pseudo_nbytes = receive(sock, (void *) &pseudo);
        nbytes = receive(sock, (void *) &recvbuffer);
        ssize_t timestamp_nbytes = receive(sock, (void *) &recvtimestamp);
        if (nbytes > 0 && timestamp_nbytes > 0 && pseudo_nbytes > 0) {
            printf("User %s sent %s at : %s\n", pseudo, recvbuffer, recvtimestamp);
            free(recvtimestamp);
            free(recvbuffer);
        }
    }
    return 0;
}

void* fssend(void *socket) {
    int sock = *(int*)socket;
    char buffer[BUFF_SIZE];
    char timestamp[TIMESTAMP_SIZE];

    ssize_t nbytes = 1;
    ssize_t timestamp_nbytes = 1;

    while (nbytes != 0 && timestamp_nbytes > 0 && printf("Entrez votre message: \n") && fgets(buffer, BUFF_SIZE, stdin)) {
        // on initialize le temps
        time_t now = time(NULL);
        strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));

        //Longueur du message size_t
        size_t len = strlen(buffer);

        // Supprimer le \n
        buffer[len - 1] = '\0';

        // On garde la même taille de string pour explicitement envoyer le '\0'
        nbytes = ssend(sock, buffer, len);
        timestamp_nbytes = ssend(sock, timestamp, strlen(timestamp));
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("You have entered %d  arguments: \n", argc);
        printf("USAGE: $ ./client <pseudo> <ip_serveur> <port> \n"),
                exit(1);
    }
    char *pseudo = argv[1];
    size_t pseudo_len = strlen(pseudo);
    if (pseudo_len < 3 || 25 < pseudo_len ) {
        printf(" <pseudo> characters quantity must be between 3 and 25 \n");
        exit(1);
    }
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

    //connection demand
    checked(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)));

    //send pseudo
    char timestamp[TIMESTAMP_SIZE];
    time_t now = time(NULL);
    strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
    ssend(sock, pseudo, pseudo_len);
    ssend(sock, timestamp, strlen(timestamp));

    // thread ids
    pthread_t tids[2];

    pthread_create(&tids[0], NULL, fssend, (void *) &sock);
    pthread_create(&tids[1], NULL, freceive, (void *) &sock);
    pthread_join(tids[0], NULL);
    pthread_join(tids[1], NULL);

    printf("Program is shutting down.\n");
    return 0;
}