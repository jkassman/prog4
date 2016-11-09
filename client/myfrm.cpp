#include <iostream>

#include "../common.hpp"

using namespace std;

int main(int argc, char **argv)
{
    //Argument handling stuff
    const char *hostname = "student03.cse.nd.edu";
    const int portNumber = 41032;

    int status;

    //Create the Server's address structure
    //Get Ip Address from name
    struct hostent *serverIp;
    serverIp = gethostbyname(hostname);
    if (!serverIp)
    {
        fprintf(stderr, "myfrm: Unknown Host %s\n", hostname);
        exit(2);
    }

    //Complete Address Structure
    struct sockaddr_in sinbad;
    bzero(&sinbad, sizeof(sinbad));
    sinbad.sin_family = AF_INET;
    bcopy(serverIp->h_addr, (char *) &sinbad.sin_addr, serverIp->h_length);
    sinbad.sin_port = htons(portNumber);

    //create TCP socket
    int tcpSock = socket(PF_INET, SOCK_STREAM, 0);
    if (tcpSock < 0)
    {
        perror("Failed to Create TCP Socket");
        exit(2);
    }

    //connect to the TCP Server
    status = connect(tcpSock, (struct sockaddr*) &sinbad, sizeof(sinbad));
    if (status < 0)
    {
        perror("Failed TCP connect()");
        close(tcpSock);
        exit(2);
    }

    char buffy[1001];
    //debug messages to make sure the connection is working
    tcpStrSend(tcpSock, "This is a test. Also, the Soviets have invaded.\n", "Could not send test");
    tcpRecv(tcpSock, buffy, 1000, "error receiving test message");
    cout << buffy;
    close(tcpSock);

    exit(0);
}
