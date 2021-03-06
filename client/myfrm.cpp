/*
  Authors: Jacob Kassman, Lauren Kuta, Matt Paulson
  netIDs: jkassman, lkuta, mpaulson
  Computer Networks: Programming Assignment 4
  myfrm.cpp
  Creates a client to access a forum server.
 */

#include <iostream>
#include <fcntl.h>

#include "../common.hpp"

using namespace std;

//Client support for the RDB operation.
//It receives the size of the board, then the name of the board.
//It then receives the file, prints it out, then deletes the file.
void clientRead(int tcpSock, int udpSock)
{
    struct sockaddr_in serverAddr;
    socklen_t serverAddr_len = sizeof(serverAddr);
    
    //receive the file size
    int recvFileSize;
    int fileSize;
    tcpRecv(tcpSock, &recvFileSize, 4, "Could not receive RDB filesize");
    fileSize = ntohl(recvFileSize);
    if (fileSize < 0)
    {
        cout << "The requested board does not exist" << endl;
        return;
    }
    //receive board name
    char boardName[PROG4_BUFF_SIZE];
    udpRecv(udpSock, &boardName, PROG4_BUFF_SIZE, &serverAddr, &serverAddr_len, 
            "RDB: Could not receive board name");

    //receive the file
    int boardFd = open(boardName, O_CREAT | O_EXCL | O_WRONLY, 0644);
    FILE* boardF = fdopen(boardFd, "w");
    if (!boardF)
    {
        cerr << "RDB: Could not open file named " << boardName << endl;
        return;
    }
    recvFile(tcpSock, boardF, fileSize, "myfrm RDB");
    
    //print the file to the screen
    fclose(boardF);
    boardF = fopen(boardName, "r");
    if (!boardF)
    {
        cerr << "RDB: Could not re-open file named " << boardName << endl;
        return;
    }
    //simple unix cat function
    int bytesPrinted = 0;
    while (bytesPrinted < fileSize)
    {
        printf("%c", (char) fgetc(boardF));
        bytesPrinted++;
    }
    fclose(boardF);

    remove(boardName);
}

//client function to support the append operation.
//sends an ack, receives the filename, then opens the file and sends its size.
//finally sends the file over.
void clientAppend(int tcpSock, int udpSock, struct sockaddr_in & sinbad)
{
    //send ack
    udpStrSend(udpSock, "ACK APN", &sinbad, sizeof(struct sockaddr),
               "Could not send basic append acknowledge");

    //receive filename
    struct sockaddr_in serverAddr;
    socklen_t serverAddr_len = sizeof(struct sockaddr);
    char filename[PROG4_BUFF_SIZE];
    udpRecv(udpSock, filename, PROG4_BUFF_SIZE, &serverAddr, &serverAddr_len,
            "APN: Could not receive filename");

    //calculate that file's filesize, and send it.
    //If the file doesn't exist, send negative filesize.
    int fileSize;
    int fileSizeToSend;
    FILE* f = fopen(filename, "r");
    if (!f)
    {
        fileSize = -1;
    }
    else
    {
        fileSize = getFileSize(f);
    }
    fileSizeToSend = htonl(fileSize);
    udpSend(udpSock, &fileSizeToSend, 4, &sinbad, sizeof(struct sockaddr),
            "APN: Could not send filesize");
    if (fileSize < 0) return;

    //actually send the file with TCP
    sendFile(tcpSock, f, fileSize, "myfrm: APN");
}

//client function to support the download (DWN) operation.
//UPD: Sends a simple ACK, receives the file size, receives the file name
//TCP: receives the file.
void clientDownload(int tcpSock, int udpSock, struct sockaddr_in & sinbad)
{
    //send simple ack
    udpStrSend(udpSock, "ACK DWN", &sinbad, sizeof(struct sockaddr), 
               "Failed to send simple download ACK");
    
    //receive file size
    struct sockaddr_in serverAddr;
    socklen_t serverAddr_len = sizeof(serverAddr);
    int fileSize;
    udpRecv(udpSock, &fileSize, 4, &serverAddr, &serverAddr_len,
            "DWN: Could not receive file size");
    fileSize = ntohl(fileSize);

    if (fileSize < 0)
    {        
        return; //server handles error messages in this case
    }

    //receive file name
    char filename[PROG4_BUFF_SIZE];
    udpRecv(udpSock, filename, PROG4_BUFF_SIZE, &serverAddr, &serverAddr_len,
            "DWN: Could not receive file name");

    //actually receive the file
    FILE* f = fopen(filename, "w");
    if (!f)
    {
        cerr << "DWN: Could not open file " << filename << endl;
        exit(1);
    }
    recvFile(tcpSock, f, fileSize, "myfrm: DWN");

    fclose(f);
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
    
    //create UDP socket
    int udpSock = socket(PF_INET, SOCK_DGRAM, 0);
    if (udpSock < 0)
    {
        perror("Failed to create UPD Socket");
        exit(2);
    }

    //initiallize UDP connection
    udpStrSend(udpSock, "Initialize UDP Session", 
               &sinbad, sizeof(struct sockaddr), "Could not send username");

    //setup complete, now we're ready to do networks stuff!

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
        //Receive prompt from server:
        struct sockaddr_in sinebard;
        socklen_t sinebardlen = sizeof(sinebard);
        //cout << "Listening for server message" << endl;
        udpRecv(udpSock, buffy, PROG4_BUFF_SIZE, &sinebard, &sinebardlen, 
                "error receiving server prompt message");
        //cout << "Received Server Message" << endl;
        //interpret what the server said
        if (!strcmp(buffy, "XIT"))
        {
            break;
        }
        else if (!strcmp(buffy, "RDB"))
        {
            clientRead(tcpSock, udpSock);
            continue;
        }else if (!strcmp(buffy, "SHT"))
        {
            cout << "The server has been shut down." << endl;
           break;
        }
        else if (!strcmp(buffy, "APN"))
        {
            clientAppend(tcpSock, udpSock, sinbad);
            continue;
        }
        else if (!strcmp(buffy, "DWN"))
        {
            clientDownload(tcpSock, udpSock, sinbad);
            continue;
        }

        //print out the server's prompt
        cout << buffy;

        //get input from the user and send it to the server:
        string userMsg;
        getline(cin, userMsg);
        udpStrSend(udpSock, userMsg.c_str(), &sinbad, sizeof(struct sockaddr), 
                   "Could not send user message");
    }

    close(udpSock);
    close(tcpSock);
    exit(0);
}
