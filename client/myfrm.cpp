#include <iostream>

#include "../common.hpp"

using namespace std;

int main(int argc, char **argv)
{
    //Argument handling
    int status;
    if (argc != 3)
    {
        fprintf(stderr, "Wrong number of arguments.\n"
                "Proper usage: myftp Server_Name Port\n");
        exit(1);
    }
    
    //grab arguments
    char *hostname = argv[1];
    char *portNumberStr = argv[2];
    int portNumber;

    //Parse the port
    status = stringToInt(&portNumber, portNumberStr, 0, INT_MAX);
    if (status == 1)
    {
        fprintf(stderr, "Error: Negative port value.\n");
    }
    else if (status == 2)
    {
        fprintf(stderr, "Error: Port value out of integer range!\n");
    }
    else if (status == 3)
    {
        fprintf(stderr, "Error: Unrecognized characters in port (%s)\n", 
                portNumberStr);
    }
    if (status) exit(1);

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
        perror("Failed to create TCP Socket");
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

    int udpSock = socket(PF_INET, SOCK_DGRAM, 0);
    if (udpSock < 0)
    {
        perror("Failed to create UPD Socket");
        exit(2);
    }

    //setup complete, now we're ready to do stuff!

    //THE FOLLOWING IS 100% DEBUGGING
    //debug messages to make sure the connection is working!
    char buffy[1001];
    tcpStrSend(tcpSock, "This is a TCP test. Also, the Soviets have invaded.\n", "Could not send TCP test");
    tcpRecv(tcpSock, buffy, 1000, "error receiving TCP test message");
    cout << "TCP Received: " << buffy << endl;
    close(tcpSock);

    socklen_t test = sizeof(struct sockaddr);

    udpStrSend(udpSock, "This is a UDP test. All systems normal.\n", &sinbad,
              test, "error sending UDP test");

    struct sockaddr_in serverbad;
    socklen_t serverlen = sizeof(struct sockaddr);
    udpRecv(udpSock, buffy, 1000, &serverbad, &serverlen, "error receiving UDP test");
    
    cout << "UDP Received: " << buffy << endl;

    exit(0);
}
