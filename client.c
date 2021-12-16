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

//#include <functional>   // fatal error: functional: No such file or directory

#include "common.h"

struct thread_args
{
    int socket;
    int r_s;
};

struct thread_args Structthread1;
struct thread_args Structthread2;

///r_or_s 0 or 1
void* receive_ssend(void *Structthread)
{
    char buffer[BUFF_SIZE];
    ssize_t nbytes = 1;

    struct thread_args *args = (struct thread_args *) Structthread;

    int *sock;  /* a char pointer with no memory allocated */
    sock = malloc(4);   /* ptr now has room for 15 characters + null terminator */
    //sock = *Structthread->socket;
    sock = &args->socket;

    int *r_s;  /* a char pointer with no memory allocated */
    r_s = malloc(4);   /* ptr now has room for 15 characters + null terminator */
    //r_s = *Structthread->r_s;
    r_s = &args->r_s;

    while (nbytes > 0
           && printf("Entrez votre message: "),
            fgets(buffer,
                  BUFF_SIZE,
                  stdin)
            )
    {
        if (*r_s == 0)
        {
            char *recvbuffer;
            nbytes = receive(*sock, (void *) &recvbuffer);
            if (nbytes > 0)
            {
                printf("Client received %s\n", recvbuffer);
                free(recvbuffer);
            }
        }

        if (*r_s == 1)
        {
        //Longueur du message size_t
        size_t len = strlen(buffer);
        // Supprimer le \n
        buffer[len - 1] = '\0';

        // On garde la même taille de string pour explicitement envoyer le '\0'
        //int ssend(int sock, void* data, size_t len) {
        nbytes = ssend(*sock, buffer, len);
        //if (nbytes_to_send > 0) {

        }
    }
}


///
/// \param argc
/// \param argv
/// \return
int main(int argc,
         char *argv[]){

    if (argc != 4)
    {
        printf("You have entered %d  arguments: \n", argc);
        printf("USAGE: $ ./client <pseudo> <ip_serveur> <port> \n"),
        exit(1);
    }

    char *pseudo = argv[1];
    size_t pseudo_len = strlen(pseudo);
    if (pseudo_len < 3 || 25 < pseudo_len )
    {
        printf(" <pseudo> characters quantity must be between 3 and 25 \n"),
        exit(1);
    }

    const char *ip = argv[2]; // 127.0.0.1
    const char *temp_port = argv[3];
    // converting char to int
    int port = conv_port(temp_port);

    int sock = checked(socket(AF_INET,
                                  SOCK_STREAM,
                                  0));
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Conversion de string vers IPv4 ou IPv6 en binaire
    checked(inet_pton(AF_INET,
                            ip,
                            &serv_addr.sin_addr));

    //connection demand
    checked(connect(sock,
                        (struct sockaddr *)&serv_addr,
                                sizeof(serv_addr)));


    //send pseudo
    ssend(sock, pseudo, pseudo_len);



    pthread_t tids[2];  // thread ids

    Structthread1.socket = sock;
    Structthread1.r_s = 0;
    Structthread2.socket = sock;
    Structthread2.r_s = 1;

    pthread_create(&tids[0], NULL, receive_ssend, (void *) &Structthread1);
    pthread_create(&tids[1], NULL, receive_ssend, (void *) &Structthread2);
    pthread_join(tids[0], NULL);
    pthread_join(tids[1], NULL);


    printf("Program is shutting down.\n");
    return 0;
}


