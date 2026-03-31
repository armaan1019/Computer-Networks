//COEN 146L : Lab3, step 1: TCP client requesting a file transfer from the server.
/*
* Name: Armaan Sharma
* Date: January 20, 2026
* Title: Lab3 - Part 1 - TCP Client
* Description: This program implements a TCP client that requests a file transfer from a server.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){
    //Get from the command line, server IP, src and dst files.
    if (argc != 5){
		printf ("Usage: %s <ip of server> <port #> <src_file> <dest_file>\n",argv[0]);
		exit(0);
    } 
    //Declare socket file descriptor and buffer
    int sockfd;
    char buf[10];

    //Declare server address to accept
    struct sockaddr_in servaddr; // IP + port structure
    
   //Declare host
   struct hostent *server; // host info structure
   
    //get hostname
    server = gethostbyname(argv[1]); 
    if (server == NULL) {
        perror("gethostbyname failed");
        exit(1);
    }

    //Open a socket, if successful, returns
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        perror("Error opening socket");
        exit(1);
    }

    //Set the server address to send using socket addressing structure
    memset(&servaddr, 0, sizeof(servaddr)); // zero the struct and safely assign values
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    memcpy(&servaddr.sin_addr.s_addr, server->h_addr, server->h_length);

    //Connect to the server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        perror("Error connecting to server");
        close(sockfd);
        exit(1);
    }
    
    //Client sends the name of the file to retrieve from the server
    write(sockfd, argv[3], strlen(argv[3]));

    //Client begins to write and read from the server
    FILE *fp = fopen(argv[4], "wb");
    if (fp == NULL){
        perror("Error opening destination file");
        close(sockfd);
        exit(1);
    }

    int bytes;
    while ((bytes = read(sockfd, buf, sizeof(buf))) > 0){
        fwrite(buf, 1, bytes, fp);
    }
    
    //Close socket descriptor
    fclose(fp);
    close(sockfd);
    
    return 0;
}

