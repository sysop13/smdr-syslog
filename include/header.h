#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <error.h>
#include <time.h>
#include <err.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <syslog.h>

#include "list.h"

#define MAX_EVENTS 1000
#define MAX_BUF    256
#define SERVER_PORT 5100
#define SERVER_HOST "192.168.7.11"

// -------------------------------------------------
/* структура узла списка */
typedef struct _node {
    void (*func)(struct _node*);
    int flag;
    int fd;
    struct timer *timer0;
    struct list_head list;
} node;


