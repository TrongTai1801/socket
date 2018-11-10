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
#include <sstream>

using namespace std;

main(int argc, char** argv)
{
    int listening_sock, new_sock;
    fd_set working_set ,master_set;
    int max_sd;
    int socket_count;

    //create socket
    listening_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_sock < 0) {
        cout << "Cannot create socket! Quiting...\n";
        return 0;
    }

    //bind the ip address and port to the socket
    sockaddr_in listening_addr;
    bzero((char*) &listening_addr, sizeof(listening_addr));
    listening_addr.sin_family = AF_INET;
    listening_addr.sin_port = htons(8888);
    listening_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listening_sock, (sockaddr*) &listening_addr, sizeof(listening_addr))) {
        cout << "Cannot bind address to the socket! Quiting...\n";
        return 0;
    }

    //listen if any client is connecting
    listen(listening_sock, 10);
    cout << "Listening...\n" << endl;

    FD_ZERO(&master_set);
    max_sd = listening_sock;
    cout << "Created a fd_set\n" << endl;

    FD_SET(listening_sock, &master_set);
    cout << "Added listening_sock to the fd_set\n" << endl;

    while (true) {
        memcpy(&working_set, &master_set, sizeof(master_set));
        socket_count = select(max_sd + 1, &working_set, NULL, NULL, NULL);
        if (socket_count < 0) {
            cout << "  select() failed\n";
            break;
        }
        for (int i = 0; i <= max_sd; i++) { //search what fd is send signal
            if (FD_ISSET(i, &working_set)) {
                if (i == listening_sock) {
                    cout << "signal from listening_socket\n";
                    //accept new connection
                    new_sock = accept(listening_sock, NULL, NULL);
                    if (new_sock < 0)
                    {
                        if (errno != EWOULDBLOCK) perror("  accept() failed");
                        break;
                    }
                    //add to master_set
                    printf("New incomming connection - %d \n", new_sock);
                    FD_SET(new_sock, &master_set);
                    cout << "Added " << new_sock << " to master_set\n";
                    //send welcome message
                    string welc = "Welcome to the Awesome Chat Server\n";
                    send(new_sock, welc.c_str(), welc.size() + 1, 0);
                    //compare with max_sd to reset it
                    if (new_sock > max_sd) max_sd = new_sock;
                    //broadcast we have new connection
                } else {
                    printf("signal from %d\n", i);
                    //new message
                    char buf[4096];
                    bzero((char*) &buf, sizeof(buf));
                    // char buf2[4096];
                    // bzero((char*) &buf2, sizeof(buf2));

                    int byteIn = recv(i, &buf, sizeof(buf), 0);
                    if (byteIn == 0) {
                        //Drop client
                        printf("Clinet %d disconected\n", i);
                        close(i);
                        FD_CLR(i, &master_set);
                    } else {
                        cout << "Client " << i << " said: " << buf << "\n";
                        ostringstream ss;
                        ss << "SOCKET #" << i << ": " << buf << "\r\n";
                        string strOut = ss.str(); 
                        //send to other clinet
                        for (int j = 0; j <= max_sd; j++) {
                            if ((j != listening_sock) && (j != i)) {
                                if (FD_ISSET(j, &master_set)) {
                                    if (send(j, strOut.c_str(), strOut.size() + 1, 0) < 0) {
                                        cout << "Cannot send message to other clinet\n";
                                    } else {
                                        cout << "Send message to " << j << endl;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
