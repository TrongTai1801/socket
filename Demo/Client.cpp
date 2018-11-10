#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>
#include <arpa/inet.h>

using namespace std;

#define MAX_REQUEST 30
#define BUFFER_LEN 1025
#define SERVER_PORT  6969

int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[BUFFER_LEN];

    //create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cout << "ERROR: cannot create socket\n";
        exit(1);
    } else {
        cout << "CLIENT: create socket successful\n";
    }

    //get server
    server = gethostbyname("localhost");
    if (server == NULL) {
        cout << "ERROR: cannot create socket\n";
        exit(1);
    }
    //set server address
    bzero((char *) &serv_addr, sizeof(serv_addr));
    bcopy((char*) server->h_addr, (char*) &serv_addr, server->h_length);
    serv_addr.sin_family = AF_INET;
    // serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(SERVER_PORT);
    printf("Connecting to address %s, port %d\n", 
        inet_ntoa(serv_addr.sin_addr), serv_addr.sin_port);
    //connect to server
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "CLIENT: connect to server is failed\n";
        exit(1);
    }

    while (1) {
        printf("Please enter the message: ");
        bzero(buffer, BUFFER_LEN - 1);
        fgets(buffer, BUFFER_LEN -1, stdin);
        //send message to server
        n = write(sockfd, buffer, strlen(buffer));
        if (n < 0) {
            cout << "ERROR: cannot write message.\n";
            exit(1);
        }
        //receive reply from server
        n = read(sockfd, buffer, BUFFER_LEN - 1);
        if (n < 0) {
            cout << "ERROR: cannot read message.\n";
            exit(1);
        }
        cout << "Server said: " << buffer << "\n";
    }
    close(sockfd);
    return 0;
}