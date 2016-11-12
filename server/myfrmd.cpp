
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

void serverCreate(int sock, string currentUser){
  struct sockaddr_in client_addr;
  socklen_t addr_len;
  char boardName[1000];
  addr_len = sizeof(client_addr);

  udpRecv(sock,boardName,1000,&client_addr,&addr_len,"myfrmd");
  printf("Board Name is:%s",boardName);

  //loop through boardVec to make sure boardName is unique

  //board b1(currentUser,boardName);
  //push to a vector of boards
}

int main(int argc, char * argv[]){
  struct sockaddr_in sin, client_addr;
  char buf[PROG4_BUFF_SIZE];
  char message[PROG4_BUFF_SIZE];
  socklen_t len, addr_len;
  int tcp_s, udp_s, ntcp_s, port, bytesRec;

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
  if((tcp_s=socket(PF_INET,SOCK_STREAM,0))<0){
    perror("myftpd:socket");
    exit(1);
  }
  //bind socket to specified address
  if((bind(tcp_s,(struct sockaddr *)&sin,sizeof(sin)))<0){
    perror("myftpd: bind");
    exit(1);
  }

 //setup passive open
  if((udp_s = socket(PF_INET,SOCK_DGRAM,0)) < 0){
    perror("simplex-talk:socket");
    exit(1);
  }

  //bind the created socket to the specified address
  if((bind(udp_s,(struct sockaddr *)&sin, sizeof(sin))) < 0) {
    perror("simplex-talk:bind");
    exit(1);
  }

  while (1)
  {
      //setup TCP and UDP client

      //listen to the socket
      if((listen(tcp_s,0))<0){
          perror("myftpd:listen");
          exit(1);
      }

      

  addr_len = sizeof(client_addr);

  printf("Hello, and Welcome to the Server of the 12st Century!\n");

  while(1){
    if((ntcp_s = accept(tcp_s,(struct sockaddr *)&sin,&len))<0){
        perror("myfrmd:accept");
        exit(1);
    }
    while(1){
      if((bytesRec=recv(ntcp_s,buf,sizeof(buf),0))==-1){
        perror("Server Received Error!");
        exit(1);
      }
      if(bytesRec==0) break; //client ^C
      if(strcmp("CRT",buf)==0){
	serverCreate(udp_s);
      }
      else if(strcmp("LIS",buf)==0){
        //serverUpload(udp_s);
      }
      else if(strcmp("MSG",buf)==0){
        //serverList(udp_s);
      }
      else if(strcmp("DLT",buf)==0){
        //serverDelete(udp_s);
      }
      else if(strcmp("RDB",buf)==0){
        //serverMKD(ntcp_s);
      }
      else if(strcmp("EDT",buf)==0){
        //serverRMD(udp_s);
      }
      else if(strcmp("APN",buf)==0){
        //serverCHD(ntcp_s);
      }
      else if(strcmp("DWN",buf)==0){
        //serverCHD(ntcp_s);
      }
      else if(strcmp("DST",buf)==0){
        //serverCHD(udp_s);
      }
      else if(strcmp("XIT",buf)==0){
        strcpy(message,"Not currently functional");
      }
      else if(strcmp("SHT",buf)==0){
        strcpy(message,"Not currently functional");
      }
      else{
        strcpy(message,"Send a correct command\n");
      }
      printf("TCP Server Received:%s\n",buf);
      tcpStrSend(ntcp_s,message,"myfrmd"); 
    }
    printf("Client Quit!\n");
    close(ntcp_s);
  }
  
  /*
  //Make the map:
  map <string, string> users;

  //Sends request for the username:
  string requestName = "Please enter your desired username: ";
  udpSend(udpSock, requestName.c_str(), requestName.length(), &sin, sizeof(struct sockaddr), "Could not send username request");

  //Receives username:
  struct sockaddr_in sine;
  socklen_t sinelen = sizeof(sine);
  char buffy[1001];
  bool exists = false;

  udpRecv(udpSock, buffy, 1000, &sine, &sinelen, "Did not receive username");
  string username = buffy;

  //Checks to see if it is a new user or existing user:
  for(map<string, string>::iterator it = users.begin(); it != users.end(); ++it) {
   if(it->first == buffy) {
    password = it->second;
    exists = true;
   }
  }

  //Requests password:
  if (exists) {
   string message = "Please enter your current password: ";
  }else{
   string message = "Welcome! Please enter the password you would like to use: ";
  }
  udpSend(udpSock, message.c_str(), message.length(), &sin, sizeof(struct sockaddr), "Could not send password request");

  //Receives password:
  udpRecv(udpSock, buffy, 1000, &sine, &sinelen, "Did not receive password information");

  //Checks to see if there is a new user or see if the password matches:

  //Sends acknowledgment to the client:

  /*
  //Wait for operation from client:
  
  close(udp_s);
  close(tcp_s);
}
