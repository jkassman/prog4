/*
  Authors: Jacob Kassman, Lauren Kuta, Matt Paulson
  netIDs: jkassman, lkuta, mpaulson
  Computer Networks: Programming Assignment 4
  myfrm.cpp
  Creates a client to access a forum server.
 */

#include <iostream>

#include "../common.hpp"

using namespace std;

void clientRead(int tcpSock)
{
    //receive the file size
    int recvFileSize;
    int fileSize;
    tcpRecv(tcpSock, &recvFileSize, 4, "Could not receive RDB filesize");
    fileSize = ntohl(recvFileSize);
    
    //receive board name
    char boardName[PROG4_BUFF_SIZE];
    tcpRecv(tcpSock, &boardName, PROG4_BUFF_SIZE, "RDB: Could not receive board name");
    
    //receive the file
    FILE* boardF = fopen(boardName, "rw");
    recvFile(tcpSock, boardF, fileSize, "myfrm RDB");

    
    //print the file to the screen
    fflush(boardF);
    int bytesPrinted = 0;
    while (bytesPrinted < fileSize)
    {
        printf("%c", fgetc(boardF));
        bytesPrinted++;
    }
    fclose(boardF);

    remove(boardName);
}

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

    //initiallize UDP connection
    udpStrSend(udpSock, "Initialize UDP Session", 
               &sinbad, sizeof(struct sockaddr), "Could not send username");

    //setup complete, now we're ready to do stuff!

    //Prompt user for operation state:
    /* Main while loop: 
       0) interpret server's prompt
       1) print out server's prompt
       2) grab input from the user
       3) send that input to the server
    */
    char buffy[PROG4_BUFF_SIZE];
    while (1)
    {
        //Receive Prompt from server:
        struct sockaddr_in sinebard;
        socklen_t sinebardlen = sizeof(sinebard);
        cout << "Listening for server message" << endl;
        udpRecv(udpSock, buffy, PROG4_BUFF_SIZE, &sinebard, &sinebardlen, 
                "error receiving server prompt message");
        cout << "Received Server Message" << endl;
        //interpret what the server said
        if (!strcmp(buffy, "XIT"))
        {
            break;
        }
        else if (!strcmp(buffy, "RDB"))
        {
            clientRead(tcpSock);
        }

        //print out the server's prompt
        cout << buffy;

        //get input from the user and send it to the server:
        string userMsg;
        cin >> userMsg;
        udpStrSend(udpSock, userMsg.c_str(), &sinbad, sizeof(struct sockaddr), 
                   "Could not send user message");
    }

    close(udpSock);
    close(tcpSock);
    exit(0);
}
