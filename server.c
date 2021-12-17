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

#define ID_LEN 25

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("You have entered %d  arguments: \n", argc);
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
    fd_set writefds;

    int clients[BUFF_SIZE];
    int nclients = 0;

    /**database indice=id value=pseudo (a voir si ca marche)*/
    //double users[1024];
    //char *pseudos[8][32] = { 0 };
    //char pseudos[8] = { 0 };


    int variableNumberOfElements = 8;
    char **pseudos;

    pseudos = malloc(variableNumberOfElements * sizeof(char*));
    for (int i = 0; i < variableNumberOfElements; i++){
        pseudos[i] = malloc((ID_LEN+1) * sizeof(char));
        strcpy(pseudos[i], "NULL");
    }


    printf("begin \n");
    for (int i = 0; i < 8; i++)
    {
        printf("%d : %s \n", i, pseudos[i]);
    }
    printf("end \n");

    /**
    int sizeof_pseudos = 8;
    char const *pseudos[8];
    for (int i = 0; i < 8; i++)
    {
        pseudos[i] = NULL;
    }
    */

    while (true)
    {
        // add new sockets to fd_set
        FD_ZERO(&readfds);
        FD_SET(master_socket, &readfds);
        int max_fd = master_socket;
        for (int i = 0; i < nclients; i++)
        {
            FD_SET(clients[i], &readfds);
            FD_SET(clients[i], &writefds);
            if (clients[i] > max_fd)
            {
                max_fd = clients[i];
            }
        }
        // wait for an activity on one of the sockets,
        // timeout is NULL
        // select with
        // one fd_set in reading,
        // one fd_set in writing
        select(max_fd + 1,
               &readfds,
               &writefds,
               NULL,
               NULL);

        if (FD_ISSET(master_socket, &readfds)) {

            // Si c'est le master socket qui a des donnees,
            // c'est une nouvelle connexion.
            clients[nclients] = accept(master_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);
            nclients++;
            //printf("connection \n");

//            for (int i = 0; i < nclients; i++)
//            {
//                char *pseudo;
//                size_t nbytez = receive(clients[i], (void *) &pseudo);
//                //users[clients[i]] = *pseudo;
//                users[clients[i]] = &pseudo;
//            }
        }
        else
        {
            // Sinon, c'est un message d'un client
            for (int i = 0; i < nclients; i++)
            {
                if (FD_ISSET(clients[i], &readfds))
                {
                    //msg received on client_sockets[i]
                    //printf("before buffer init %d : %s \n", 0, pseudos[0]);
                    //printf("before buffer init %d : %s \n", i, pseudos[i]);

                    char *buffer;
                    size_t nbytes = receive(clients[i], (void *)&buffer);
                    if (nbytes > 0)
                    {
                        printf("begin \n");
                        for (int i = 0; i < 8; i++)
                        {
                            printf("%d : %s \n", i, pseudos[i]);
                        }
                        printf("end \n");

                        if (strcmp(pseudos[i], "NULL") == 0)
                        {
                            /**not yet connected*/
                            char *pseudo = buffer;
                            //char pseudo[] = buffer;

                            //printf(" pseudo %d : %s \n", i, pseudos[i]);

                            printf("buffer: %s", buffer);
                            /** user pseudo not registered */
                            printf("begin \n");
                            for (int i = 0; i < 8; i++)
                            {
                                printf("%d : %s \n", i, pseudos[i]);
                            }
                            printf("end \n");

                            //strcpy_s(pseudos[i], *buffer);
                            strcpy(pseudos[i], buffer);
                            //pseudos[i] = *buffer;
                            //printf("pseudo = %s", pseudo);

                            printf("%s connected \n", pseudo);

                            //user pseudo registered
                            printf("begin \n");
                            for (int i = 0; i < 8; i++)
                            {
                                printf("%d : %s \n", i, pseudos[i]);
                            }
                            printf("end \n");
                        }
                        else
                        {
                            // printf("User %s a dit %s\n", users[clients[i]], buffer);
                            printf("%s \n", pseudos[i]);
                            printf("a dit %s\n", buffer);
                            size_t len_pseudo = strlen(pseudos[i]);

                            for (int j = 0; j < nclients; j++)
                            {
                                if (FD_ISSET(clients[j], &writefds))
                                {
                                    printf("sending %s \n", buffer);
                                    ssend(clients[j], pseudos[i], len_pseudo);
                                    ssend(clients[j], buffer, nbytes);
                                }
                            }
                        }
                        free(buffer);
                        //free(pseudo);

                    }

                    else
                    {
                        close(clients[i]);
                        // On deplace le dernier socket a la place de libre
                        // pour ne pas faire de trou.
                        clients[i] = clients[nclients - 1];
                        nclients--;
                    }
                }
            }
        }
    }
    return 0;
}