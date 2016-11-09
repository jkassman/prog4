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
 
    //Client sends the username:
    /* 
     string username;
     char userResponseBuff[1000];
     char userResponseBuff2[1000];
     cout << "Please enter your desired username: " << endl;
     cin >> username;
     int len = strlen(username.c_str())+1;
     udpSend(udpSock, username.c_str(), len, (struct sockaddr*)&udpSock, sizeof(struct sockaddr), "Could not send username");

    //Receives call from server determining whether the username has been taken or not:
     udpRecv(udpSock, userResponseBuff, 999, (struct sockaddr*)&client_socket, &addr_len, "Could not receive username confirmation");
     cout << userResponseBuff << endl;

    //If ok, client sends the password:
    cout << "Please enter your desired password << endl;
    cin >> password;
    int len2 = strlen(password.c_str())+1;
    udpSend(udpSock, password.c_str(), len2, (struct sockaddr*)&udpSock, sizeof(struct sockaddr), "Could not send password");

    //Ack that the user is now in:
    //"Created a new account" or "username matched"
    //or username does not match
    udpRecv(udpSock, userResponseBuff2, 999, (struct sockaddr*)&client_socket, &addr_len, "Could not receive password information");
    if(strcmp(userResponseBuff2, "Username does not match password given.") != 0) {
      cout << userResponseBuff2 << endl;
      exit(1);
    }else{
      cout << userResponseBuff2 << endl;
    }
    */
    //Prompt user for operation state:

    exit(0);
}
