#ifndef __COMMON_H
#define __COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

///
/// \param temp_port
/// \return
int conv_port(const char *temp_port) {
    char *p;
    int num;
    errno = 0;
    long port = strtol(temp_port, &p, 10);

    // Check for errors: e.g., the string does not represent an integer
    // or the integer is larger than int
    if (errno != 0 || *p != '\0' || port > INT_MAX || port < INT_MIN) {
        // Put here the handling of the error, like exiting the program with
        // an error message
    } else {
        // No error
        num = port;
    }
    return num;
}

///
/// \param ret return number of calling_function to be checked
/// \param calling_function
/// \return
int _checked(int ret, char* calling_function) {
    if (ret < 0) {
        perror(calling_function);
        exit(EXIT_FAILURE);
    }
    return ret;
}

// The macro allows us to retrieve the name of the calling function
#define checked(call) _checked(call, #call)

/**
 * @brief Send data under the form <size_t len><...data>
 * Function name is 'ssend' instead of 'send' because the latter already exists.
 */

///
/// \param sock socket to send data to
/// \param data to be sent
/// \param len of data
/// \return
int ssend(int sock, void* data, size_t len) {
    checked(write(sock, &len, sizeof(len)));

//    time_t mytime = time(NULL);
//    char * timestamp = ctime(&mytime);
//    timestamp[strlen(timestamp)-1] = '\0';

//    char timenow[20];
//    time_t now = time(NULL);
//    strftime(timenow, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
//    endata = timenow+data;
//    printf("%s", endata);
    return checked(write(sock, data, len));
}

/**
 * @brief Receive data under the form <size_t len><data...>.
 */

///
/// \param sock socket to receive data from
/// \param dest destination
/// \return
size_t receive(int sock, void** dest) {
    size_t nbytes_to_receive;
    if (checked(read(sock, &nbytes_to_receive, sizeof(nbytes_to_receive))) == 0) {
        // Connection closed
        return 0;
    };
    unsigned char* buffer = malloc(nbytes_to_receive);
    if (buffer == NULL) {
        fprintf(stderr, "malloc could not allocate %zd bytes", nbytes_to_receive);
        perror("");
        exit(1);
    }
    size_t total_received = 0;
    while (nbytes_to_receive > 0) {
        size_t received = checked(read(sock, &buffer[total_received], nbytes_to_receive));
        if (received < 0) {
            return total_received;
        }
        total_received += received;
        nbytes_to_receive -= received;
    }
    *dest = buffer;
    return total_received;
}

#endif  // __COMMON_H
