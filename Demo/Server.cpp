#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

using namespace std;

#define PORT 8888
#define MAX_REQUEST 30
#define BUFFER_LEN 256

int main(int argc, char const *argv[])
{    
    int sockfd, newsockfd, n;
    socklen_t clilen;
    char buffer[BUFFER_LEN];
    struct sockaddr_in ser_add, cli_add;

    //create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cout << "ERROR: cannot create socket\n";
        exit(1);
    } else {
        cout << "SERVER: create socket successful\n";
    }

    //init server address
    bzero((char*) &ser_add, sizeof(ser_add)); //reset ser_add
    ser_add.sin_family = AF_INET;
    ser_add.sin_addr.s_addr = INADDR_ANY; //inet_addr(address);
    ser_add.sin_port = htons(PORT);

    //attach socker to local address   
    if (bind(sockfd, (struct sockaddr*) &ser_add, sizeof(sockaddr)) < 0) {
        cout << "ERROR: error on binding\n";
        exit(1);
    } else {
        cout << "SERVER: bind to local address socket successful\n";
    }

    //listen
    listen(sockfd, MAX_REQUEST); // maximum request pending in queue is 30
    cout << "Server is listening...\n";

    while (1) {
        //accept
    clilen = sizeof(cli_add);
    newsockfd = accept(sockfd, (struct sockaddr*) &cli_add, &clilen);
    if (newsockfd < 0) {
        cout << "ERROR: Error on accept\n";
        exit(1);
    }
    //read message from clinet
    bzero(buffer, BUFFER_LEN);
    n = read(newsockfd, buffer, BUFFER_LEN - 1);
    if (n < 0) cout << "ERROR: cannot read from socket\n";
    cout << "Clinet said: " << buffer << "\n";
    //reply message to client
    n = write(newsockfd, "I've got message.", 30);
    if (n < 0) cout << "ERROR: cannot write to socket\n";
    close(newsockfd);
    close(sockfd);
    }

    return 0;
}