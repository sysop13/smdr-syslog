#include "include/header.h"

int epollfd; //я есть епул

struct list_head head; //список клиентов (да и сервер там же)

char buf [MAX_BUF];

int log_level = LOG_INFO;
int log_facility = LOG_USER;

/* создает клиента */
node* create_client(){
    node* client = (node*) malloc(sizeof(node));
    memset(client,0x0,sizeof *client);
    return client;
}

/* удаляет fd из епула и закрывает сокет */
void del (node* client){
    int fd = client->fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL) == -1) err(EXIT_FAILURE,"epoll_ctl: listen_sock");
    list_del(&client->list);
    free(client);
    close(fd);
}

/* добавляет fd в епулл и ссылку на функцию-обработчик в клиентский массив */
int add (node* client){
    int fd;
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = client;
    fd = client->fd;
    //printf("client fd=%d\n",fd);
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) err(EXIT_FAILURE,"epoll_ctl: listen_sock");
    list_add(&client->list, &head);
    return 0;
}

// -------------------------------------------------
/* строку в строчные */
void to_l(char** buf, int len, char* out){
}

// -------------------------------------------------
/* разбиение строки на лексемы */
int strok2(char *str, char **lexem){
    int i = 0;
    char * sp;
    sp=str;
    while(*sp!=0x0){
        while(*sp!=0x0 && *sp==0x20 ) sp++;
        lexem[i++]=sp;
        while(*sp!=0x0 && *sp!=0x20 ) sp++;
        if(*sp==0x0) break;
        *sp=0x0;
        sp++;
    }
    return i;
}

void func(node*);


// -------------------------------------------------
/* инициализируем клиента */
void init(){
    int listen_sock, yes=1;
    struct sockaddr_in addr;

    /* данные для эхо сервера */
    addr.sin_family = PF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_HOST);


    listen_sock = socket(PF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0); //set nonblocking
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); //reusable server socket
    connect(listen_sock, (struct sockaddr *)&addr, sizeof(addr));
    //добавили fd эхо сервера в епулл
    node* server = create_client();
    server->fd = listen_sock;
    server->func = func;
    server->flag = -1;
    add(server);
}

// -------------------------------------------------
/* stuff, который будет делать клиент */
void func(node *tmp){
    int j;
    memset (buf, 0, sizeof(buf));

    j = recv(tmp->fd, &buf,sizeof buf, 0); // получаем строку от сервера
    if(j <= 0){
	del(tmp);
	printf("sleep\n");
	sleep(rand()%5+1);
	init();
	return;
	}
    if(strlen(buf) > 2 ) {
	buf[j]='\0';
	buf[j-1]='\0';
	printf ("%s", buf);
	buf[strlen(buf) - 1] = 0;
	syslog( log_level, "%s", buf );
    }
}



// -------------------------------------------------
int main() {
    struct epoll_event events[MAX_EVENTS];
    int i,nfds;

    // инициализация списка
    INIT_LIST_HEAD(&head);

    //создали епулл
    epollfd = epoll_create(MAX_EVENTS); if (epollfd == -1) err(EXIT_FAILURE,"epoll_create1");

    init();

    openlog( "[smdr-syslog]", 0, log_facility);
    syslog( log_level, "%s", "smdr-syslog starting" );
    
    /* бежим по событиям епула и вызываем соответсвующую функцию (для клиента - клиентскую, для сервера - серверную) */
    for (;;) {
	node* tmp; 
	nfds = epoll_wait(epollfd, events, MAX_EVENTS, 1000); if (nfds == -1) err(EXIT_FAILURE,"epoll_wait");
	for (i = 0; i < nfds; i++){
	    tmp=(node*)events[i].data.ptr; 
	    if(events[i].events & EPOLLIN) {
		tmp->func(tmp);
	    }
	}
    }
}
