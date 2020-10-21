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

void get_html(int connfd, char *requested_file, int file_size);
void get_image(int connfd, FILE *requested_file, char *extension);

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
    
    if(strstr(get_request, ".") != NULL){





    if(strcmp(get_request,"/socketcluster/")==0){
        return NULL;
    }

    //get extension of file
    char *extension;
    char *parse;
    char get_request_copy[MAXBUF];
    strcpy(get_request_copy, get_request);

    //get first part of request
    parse = strtok(get_request_copy, ".");

    
if (strcmp(get_request,"/")==0){
    extension = "html" ;     
        }//if
    else{    
    //get extension
    extension = strtok(NULL, ".");
}//else for extension


    //handle further parsing when needed
    if(
        strcmp(extension,"mousewheel-3") == 0 || 
        strcmp(extension,"fancybox-1") == 0 || 
        strcmp(extension,"4") == 0 ||
        strcmp(extension,"easing-1") == 0
        )
    {
        if(strcmp(get_request,"jquery-fancybox-1.3.4.css") == 0){
            strcpy(extension, "css");
        }
        else if(strcmp(get_request,"jquery.fancybox-1.3.4.pack.js")){
            
            strcpy(extension, "js");
        }

        else{
            strcpy(extension, "js");
        }
    }//further parsing



    
    //char *extension_copy;
    //strcpy(extension_copy, extension);
    //add back extension so get_request still good
    
    //strcat(get_request, extension_copy);
printf("SEGS AFTER\n");
printf("%s has thread id %o and extension %s \n", get_request, pthread_self, extension);
printf("PRINT THIS\n");
printf("handling js");


    FILE *fp;

    //load index page
    if(strcmp(get_request, "/")==0){
    
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
    get_html(connfd, file_contents, file_size);
    }//if index

    //handle images
    else if(
        strcmp(extension,"png") == 0 || 
        strcmp(extension,"gif") == 0 || 
        strcmp(extension,"jpg") == 0 ||
        strcmp(extension,"ico") == 0
        )
    {
    //adds . to files so they can be accessed
    //explored trying to change cd to root but seems like not necessary    
    char period[MAXBUF];
    strcpy(period, ".");
    char get_request_cur_dir[MAXBUF];
    strcat(period, get_request);
    strcpy(get_request_cur_dir, period);

    //open file in binary mode
    fp = fopen(get_request_cur_dir, "rb");
    if(fp == NULL){
        printf("File not found!\n");
        return NULL;
    }

    get_image(connfd, fp, extension);
    //printf("%s\n",file_contents);*/
    //get_image(connfd, fp);
    } //image elif

    //handle html
    else if (
        strcmp(extension, "html") == 0 ||
        strcmp(extension, "html~") == 0 ||
        strcmp(extension, "txt") == 0
        )
    {

    //adds . to files so they can be accessed
    //explored trying to change cd to root but seems like not necessary
    char period[MAXBUF];
    strcpy(period, ".");
    char get_request_cur_dir[MAXBUF];
    strcat(period, get_request);
    strcpy(get_request_cur_dir, period);
    printf("handling html");

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
    get_html(connfd, file_contents, file_size);
    
   }//else if html

//handle js and css
    else if(
        strcmp(extension,"js") == 0 || 
        strcmp(extension,"css") == 0
        )
    {


    //adds . to files so they can be accessed
    //explored trying to change cd to root but seems like not necessary
    char period[MAXBUF];
    strcpy(period, ".");
    char get_request_cur_dir[MAXBUF];
    strcat(period, get_request);
    strcpy(get_request_cur_dir, period);
    

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
    }//while
    int file_size;
    file_size= strlen(file_contents);
    
    get_js(connfd, file_contents, file_size);

    }//js and css elif
   else if(strcmp(extension,NULL)==0){
    printf("extension was NULL \n");
   }
   else{
    printf("saved seg\n");
   }
    
   
    close(connfd);
    return NULL;

}//nested if
}//thread
void get_html(int connfd, char *requested_file, int file_size) 
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
    //printf("server returning a http message with the following content.\n%s\n",httpmsg);
    write(connfd, httpmsg,strlen(httpmsg));
    
}

//source: https://stackoverflow.com/questions/15445207/sending-image-jpeg-through-socket-in-c-linux
void get_image(int connfd, FILE *requested_file, char *extension) 
{   
    char *sendbuf;
    int file_size;
    char *binary_data;
    char buf[MAXLINE]; 
    char httpmsg[]="HTTP/1.1 200 Document Follows\r\nContent-Type:image/";
    char after_extension[] = "\r\nContent-Length:";

    //add type of image
    strcat(httpmsg, extension);
    strcat(httpmsg, after_extension);



    
    char after_content_length[]="\r\n\r";
    char content_length[MAXLINE];
    
    fseek (requested_file, 0, SEEK_END);

    file_size = ftell(requested_file);
    printf("file size is %d\n",file_size );
    rewind(requested_file);         
    sendbuf = (char*) malloc (sizeof(char)*file_size);
    size_t result = fread(sendbuf, 1, file_size, requested_file);

  /*  for(int i = 0; i<file_size; i++){
         printf("%x\n",sendbuf[i] );
        strcat(binary_data);
}*/
    //convert file size int to string
    itoa(file_size, content_length, 10);
    
    strcat(httpmsg, content_length);
    strcat(httpmsg, after_content_length);
    strcat (httpmsg, "Connection: keep-alive\r\n\r\n");

    //send http header
    write(connfd, httpmsg,strlen(httpmsg));

    int buf_index = 0;
    int packets = file_size / MAXBUF;
    
    //send binary data
    write(connfd, sendbuf, file_size);


    /*for(int i = 0; i < packets; i++){
    write(connfd,(*sendbuf)+buf_index,MAXBUF);
    buf_index = buf_index + MAXBUF;
}//for*/
    
}

void get_js(int connfd, char *requested_file, int file_size)  
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
    //printf("server returning a http message with the following content.\n%s\n",httpmsg);
    write(connfd, httpmsg,strlen(httpmsg));
    
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