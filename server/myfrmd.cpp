
/*
  Authors: Jacob Kassman, Lauren Kuta, Matt Paulson
  netIDs: jkassman, lkuta, mpaulson
  Computer Networks: Programming Assignment 4
  myfrmd.cpp
  ???Creates a TCP server able to receive several commands from a client.???
 */

#include "../common.hpp"
#include <string>
#include <map>

int main(int argc, char * argv[]){
  struct sockaddr_in sin;
  //char buf[PROG3_BUFF_SIZE];
  char message[PROG3_BUFF_SIZE];
  socklen_t len;
  int s, new_s, port;

  //check for correct number of arguments and assign arguments to variables
  if (argc==2){
      int convertStatus = stringToInt(&port, argv[1], 0, INT_MAX);
      switch (convertStatus) {
      case 1:
          fprintf(stderr, "Error: Negative port value.\n");
          exit(1);
      case 2:
          fprintf(stderr, "Error: Port value out of integer range!\n");
          exit(1);
      case 3:
          fprintf(stderr, "Error: Unrecognized characters in port (%s)\n",
                  argv[1]);
          exit(1);
      default:
          //Do nothing, successfully parsed the port
          break;
      }
  }
  else{
    fprintf(stderr, "usage: myftpd [Port]\n");
    exit(1);
  }

  /*build address data structure */
  bzero((char*)&sin,sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(port);

  /*setup passive open*/
  if((s=socket(PF_INET,SOCK_STREAM,0))<0){
    perror("myftpd:socket");
    exit(1);
  }
  //bind socket to specified address
  if((bind(s,(struct sockaddr *)&sin,sizeof(sin)))<0){
    perror("myftpd: bind");
    exit(1);
  }
  //listen to the socket
  if((listen(s,0))<0){
    perror("myftpd:listen");
    exit(1);
  }

  printf("Hello, and Welcome to the Server of the 12st Century!\n");
  
  if((new_s = accept(s,(struct sockaddr *)&sin,&len))<0){
      perror("myfrmd:accept");
      exit(1);
  }

  tcpRecv(new_s, message, sizeof(message), "myfrmd");
  tcpStrSend(new_s, "Test", "myfrmd");

  printf("Received:%s",message);
  
  //Make the map and other variables:
  map <string, string> users;
  struct sockaddr_in sine;
  socklen_t sinelen = sizeof(sine);
  char buffy[1001];
  bool exists = false;
  string password, username, messageSend;

  //Sends request for the username:
  string requestName = "Please enter your desired username: ";
  udpSend(udpSock, requestName.c_str(), requestName.length(), &sin, sizeof(struct sockaddr), "Could not send username request");

  //Receives username:
  udpRecv(udpSock, buffy, 1000, &sine, &sinelen, "Did not receive username");
  username = buffy;

  //Checks to see if it is a new user or existing user:
  for(map<string, string>::iterator it = users.begin(); it != users.end(); ++it) {
   if(it->first == username) {
    password = it->second;
    exists = true;
   }
  }

  //Requests password:
  if (exists) {
  }else{
    messageSend = "Welcome! Please enter the password you would like to use: ";
  }
  udpSend(udpSock, messageSend.c_str(), messageSend.length(), &sin, sizeof(struct sockaddr), "Could not send password request");

  //Receives password:
  udpRecv(udpSock, buffy, 1000, &sine, &sinelen, "Did not receive password information");
  password = buffy;

  //Checks to see if there is a new user or see if the password matches:
  if (exists) {
    if(password = users[username]) {
      messageSend = "The passwords matched! You have successfully logged in."
    }else{
      messageSend = "The entered password was incorrect."
    }
  }else{
    users[username] = password;
    message = "Account setup has been completed. Welcome to 21st Century Forums!"
  }  

  //Sends acknowledgment to the client:
  udpSend(udpSock, messageSend.c_str(), messageSend.length(), &sin, sizeof(struct sockaddr), "Could not send log in acknowledgement.");

  //Wait for operation from client:
  
  close(s);
}
