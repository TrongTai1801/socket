#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace std;

main(int argc, char** argv)
{
    //create socket
    int listening_sock;
    listening_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_sock < 0) {
        cout << "Cannot create socket! Quiting...";
        return 0;
    }

    //bind the ip address and port to the socket
    sockaddr_in listening_addr;
    bzero((char*) &listening_addr, sizeof(listening_addr)); //reset listening_addr
    listening_addr.sin_family = AF_INET;
    listening_addr.sin_port = htons(8888);
    listening_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listening_sock, (sockaddr*) &listening_addr, sizeof(listening_addr))) {
        cout << "Cannot bind address to the socket! Quiting...";
        return 0;
    }

    //listen if any client is connecting
    listen(listening_sock, 10);

    //wait for connection
    sockaddr_in client_addr;
    socklen_t client_size = sizeof(client_addr);

    int client_sock = accept(listening_sock, (struct sockaddr*) &client_addr, &client_size);

    char clinet_host[NI_MAXHOST];
    char client_port[NI_MAXSERV];

    bzero((char*) &clinet_host, sizeof(clinet_host));
    bzero((char*) &client_port, sizeof(client_port));

    if (!getnameinfo((struct sockaddr*) &client_addr, sizeof(client_addr), clinet_host, NI_MAXHOST, 
            client_port, NI_MAXSERV, 0)) {
        cout << clinet_host << " connected on port " << client_port << endl; 
    } else {
        inet_ntop(AF_INET, &client_addr.sin_addr, clinet_host, NI_MAXHOST);
        cout << clinet_host << " connected in port " << htons(client_addr.sin_port) << endl;
    }
    
    //while loop: accept and  echo message back to client
    char buf[4096];

    while(true) {
        bzero((char*) &buf, sizeof(buf));

        //wating for clinet send to socket
        int byteReceive = recv(client_sock, &buf, 4096, 0);
        if (byteReceive < 0) {
            cout << "Error in recv(). Quiting...";
            break;
        }
        if (byteReceive == 0) {
            cout << "Client disconected!" << endl;
            break;
        }
        if (byteReceive > 0) {
            cout << "Clinet said: "<< buf << endl;
        }
        //echo message back to client
        send(client_sock, buf, byteReceive + 1, 0);
    }

    //close listening socket
    close(listening_sock);

    //shuwdown winsock
    return 0;
}
