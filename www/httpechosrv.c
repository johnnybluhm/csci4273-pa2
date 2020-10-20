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

char* itoa(int value, char* result, int base);

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

    char s[100];
    printf("%s\n", getcwd(s, 100)); 
    int connfd = *((int *)vargp);
    free(vargp);
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
    char files[MAXLINE];
    strcpy(files, "./files");
    printf("%s\n", get_request);
    FILE *fp;





    if(strcmp(get_request, "/")==0){
        printf("load index\n");

        fp = fopen("index.html", "r");

    //copy file contents to string file_contents
    char file_contents[MAXLINE];
    int i =0;
    char c;
    int file_size;
    c = fgetc(fp);
    while(c != EOF){
        file_contents[i] = c;
        i++;
        c =fgetc(fp);
    }
    file_size= strlen(file_contents);

    printf("here index eneters welcome");
    welcome(connfd, file_contents, file_size);



    }//if

    else{
    //add ./files to get_request
   /* strcat(files,get_request);
    strcpy(get_request, files);*/

    //adds . to files so they can be accessed
    char period[MAXBUF];
    strcpy(period, ".");
    char get_request_cur_dir[MAXBUF];
    strcat(period, get_request);
    strcpy(get_request_cur_dir, period);
    printf("get request is %s\n", get_request);

    fp = fopen(get_request_cur_dir, "r");
    if(fp == NULL){
        printf("File not found!\n");
        return NULL;
    }

    //copy file contents to string file_contents
    char file_contents[MAXLINE];
    int i =0;
    char c;
    c = fgetc(fp);
    while(c != EOF){
        file_contents[i] = c;
        i++;
        c =fgetc(fp);
    }
    int file_size;
    file_size= strlen(file_contents);
    //printf("%s\n",file_contents);
    welcome(connfd, file_contents, file_size);
    
   }//else
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

void welcome(int connfd, char *requested_file, int file_size) 
{

    
    char buf[MAXLINE]; 
    char httpmsg[]="HTTP/1.1 200 Document Follows\r\nContent-Type:text/html\r\nContent-Length:";
    char after_content_length[]="\r\n\r\n";
    char content_length[MAXLINE];
    
    //convert file size int to string
    itoa(file_size, content_length, 10);
    
    strcat(httpmsg, content_length);
    strcat(httpmsg, after_content_length);
    

    //add file contents to http header
    strcat(httpmsg, requested_file);
    printf("server returning a http message with the following content.\n%s\n",httpmsg);
    write(connfd, httpmsg,strlen(httpmsg));
    
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

//function to convert into to string
//source:https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c/23840699#:~:text=You%20can%20use%20itoa(),to%20convert%20any%20value%20beforehand.
/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char* itoa(int value, char* result, int base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}