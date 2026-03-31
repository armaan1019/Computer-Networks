//COEN 146L : Lab3, step 3: concurrent TCP server that accepts and responds to multiple client connection requests, each requesting a file transfer
/*
* Name: Armaan Sharma
* Date: Jan 20, 2026
* Title: Lab3 - Part 3 - Concurrent TCP Server
* Description: This program implements a concurrent TCP server that accepts and responds to multiple client connection requests, each requesting a file transfer.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

//Define the number of clients/threads that can be served
#define N 100
int threadCount = 0;
pthread_t clients[N]; //declaring N threads

 //Declare socket and connection file descriptors.
 int sockfd, connfd;

 //Declare receiving and sending buffers of size 10 bytes
   char sendBuff[10];

 //Declare server address to which to bind for receiving messages and client address to fill in sending address
   struct sockaddr_in servAddr, clienAddr;

//Connection handler (thread function) for servicing client requests for file transfer
void* connectionHandler(void* sock){
   //declare buffer holding the name of the file from client
   char fileName[100];
   
   //Connection established, server begins to read and write to the connecting client
   printf("Connection Established with client IP: %s and Port: %d\n", inet_ntoa(clienAddr.sin_addr), ntohs(clienAddr.sin_port));
   
	//receive name of the file from the client
   int connfd = *((int *)sock);
   free(sock); // sock is a pointer to the client socket descriptor
   read(connfd, fileName, sizeof(fileName));
   printf("Client requested file: %s\n", fileName);
	
   //open file and send to client
   FILE *fp = fopen(fileName, "rb");
   if (fp == NULL){
       perror("File not found");
       close(connfd);
       return NULL;
   }
     
   //read file and send to connection descriptor
   int bytes;
   while ((bytes = fread(sendBuff, 1, sizeof(sendBuff), fp)) > 0){
       write(connfd, sendBuff, bytes);
   }
   
   printf("File transfer complete\n");
   
   //close file
   fclose(fp);
    
   //Close connection descriptor
   close(connfd);

   pthread_exit(0);
}


int main(int argc, char *argv[]){
 //Get from the command line, port number
 if (argc != 2){
	printf ("Usage: %s <port #> \n",argv[0]);
	exit(0);
 } 
 //Open a TCP socket, if successful, returns a descriptor
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0){
         perror("Error opening socket");
         exit(1);
   }

 //Setup the server address to bind using socket addressing structure
   servAddr.sin_family = AF_INET;
   servAddr.sin_addr.s_addr = INADDR_ANY;
   servAddr.sin_port = htons(atoi(argv[1]));

 //bind IP address and port for server endpoint socket
   if (bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0){
      perror("Error binding socket");
      close(sockfd);
      exit(1);
   }

 // Server listening to the socket endpoint, and can queue 5 client requests
   if (listen(sockfd, 5) < 0){
      perror("Error listening on socket");
      close(sockfd);
      exit(1);
   }
 printf("Server listening/waiting for client at port 5000\n");
 
 while (1){
   //Server accepts the connection and call the connection handler
   socklen_t clienLen = sizeof(clienAddr);
   connfd = accept(sockfd, (struct sockaddr *)&clienAddr, &clienLen);
   if (connfd < 0){
       perror("Error accepting connection");
       close(sockfd);
       exit(1);
   }
   
   int *newConnfd = malloc(sizeof(int)); // each thread needs its own copy of connfd
   *newConnfd = connfd;
   pthread_create(&clients[threadCount], NULL, connectionHandler, newConnfd);
   threadCount++;
 }

 for(int i = 0; i < threadCount; i++){
      pthread_join(clients[i], NULL);
 }
 return 0;
}