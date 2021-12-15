//
// Created by bourg on 15-12-21.
//
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
#include "test.h"


char pseudos[8][32] = { 0 };
char name[32] = "orange";
strcpy_s(pseudos[1], name);
printf("%s", pseudos[1]);