/* 
 * tcpechosrv.c - A concurrent TCP echo server using threads
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>      /* for fgets */
#include <strings.h>     /* for bzero, bcopy */
#include <unistd.h>      /* for read, write */
#include <sys/socket.h>  /* for socket use */
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAXLINE  8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  1024  /* second argument to listen() */

int open_listenfd(int port);
void echo(int connfd);
void *thread(void *vargp);

int main(int argc, char **argv) 
{
    int listenfd, *connfdp, port, clientlen=sizeof(struct sockaddr_in);
    struct sockaddr_in clientaddr;
    pthread_t tid; 

    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(0);
    }
    port = atoi(argv[1]);

    listenfd = open_listenfd(port);
    printf("listening on port %d\n", port);
    while (1) {    
	connfdp = malloc(sizeof(int));
	*connfdp = accept(listenfd, (struct sockaddr*)&clientaddr, &clientlen);
	pthread_create(&tid, NULL, thread, connfdp);
    }
}

/* thread routine */
void * thread(void * vargp) 
{  
    int connfd = *((int *)vargp);
    pthread_detach(pthread_self()); 
    size_t n;
  
    char request[MAXLINE];


    //read from browser
    char *request_header;
    n = read(connfd, request, MAXLINE);

    //get first line of http request
    //gives string <request type> </wherever> <HTTP/1.1>
    request_header = strtok(request, "\n");  
    
    char *request_header_token;
    request_header_token = strtok(request_header," ");

    //parse string further
    char *get_request;
    get_request = strtok(NULL, " ");

    //we now have user file request
    printf("token is %s\n",get_request );
    char files[MAXLINE];
    strcpy(files, "./files");
    printf("files is %s\n",files );
    FILE *fp;
    
    //add ./files to get_request
    strcat(files,get_request);
    strcpy(get_request, files);
    printf("get request is %s\n", get_request);
    fp = fopen(get_request, "r");
    if(fp == NULL){
        printf("File not found!\n");
    }



    free(vargp);
   
    //echo(connfd);
   
    close(connfd);
    return NULL;
}

/*
 * echo - read and echo text lines until client closes connection
 */
void echo(int connfd) 
{

    size_t n; 
    char buf[MAXLINE]; 
    char httpmsg[]="HTTP/1.1 200 Document Follows\r\nContent-Type:text/html\r\nContent-Length:32\r\n\r\n<html><h1>Hello CSCI4273 Course!</h1>"; 
    
    n = read(connfd, buf, MAXLINE);
    printf("server received the following request:\n%s\n",buf);
    strcpy(buf,httpmsg);
    printf("server returning a http message with the following content.\n%s\n",buf);
    write(connfd, buf,strlen(httpmsg));
    
}

void welcome(int connfd, char *requested_file) 
{

    
    char buf[MAXLINE]; 
    char httpmsg[]="HTTP/1.1 200 Document Follows\r\nContent-Type:text/html\r\nContent-Length:32\r\n\r\n"; 

    
    
    printf("server received the following request:\n%s\n",buf);
    strcpy(buf,httpmsg);
    printf("server returning a http message with the following content.\n%s\n",buf);
    write(connfd, buf,strlen(httpmsg));
    
}

void test(int connfd) 
{
    size_t n; 
    char buf[MAXLINE]; 
    char httpmsg[]="HTTP/1.1 200 Document Follows\r\nContent-Type:text/html\r\nContent-Length:32\r\n\r\n<html><h1>Hello CSCI4273 Course!</h1>"; 

    n = read(connfd, buf, MAXLINE);
    printf("server received the following request:\n%s\n",buf);
    strcpy(buf,httpmsg);
    printf("server returning a http message with the following content.\n%s\n",buf);
    write(connfd, buf,strlen(httpmsg));
    
}

/* 
 * open_listenfd - open and return a listening socket on port
 * Returns -1 in case of failure 
 */
int open_listenfd(int port) 
{
    int listenfd, optval=1;
    struct sockaddr_in serveraddr;
  
    /* Create a socket descriptor */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    /* Eliminates "Address already in use" error from bind. */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
                   (const void *)&optval , sizeof(int)) < 0)
        return -1;

    /* listenfd will be an endpoint for all requests to port
       on any IP address for this host */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET; 
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    serveraddr.sin_port = htons((unsigned short)port); 
    if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0)
        return -1;
    return listenfd;
} /* end open_listenfd */

