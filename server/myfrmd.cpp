
/*
  Authors: Jacob Kassman, Lauren Kuta, Matt Paulson
  netIDs: jkassman, lkuta, mpaulson
  Computer Networks: Programming Assignment 4
  myfrmd.cpp
  ???Creates a TCP server able to receive several commands from a client.???
 */

#include <string>
#include <map>
#include <iostream>
#include <vector>

#include "../common.hpp"
#include "board.hpp"

using namespace std;

bool doesBoardExist(string boardName, vector<board> & boardVec)
{
    vector<board>::iterator it;
    for (it = boardVec.begin(); it != boardVec.end(); ++it)
    {
        if (boardName == it->name)
        { 
            return true;
        }
    }
    return true;
}

string serverCreate(int sock, string currentUser, vector<board> & boardVec, sockaddr_in & sin){
  struct sockaddr_in client_addr;
  socklen_t addr_len;
  char boardName[1000];
  addr_len = sizeof(client_addr);

  udpStrSend(sock, "Please enter a name for the new board:", &sin, sizeof(struct sockaddr),"Could not send request for board name");

  udpRecv(sock,boardName,1000,&client_addr,&addr_len,"myfrmd");

  //loop through boardVec to make sure boardName is unique
  vector<board>::iterator it;
  bool nameExists = false;
  for (it = boardVec.begin(); it != boardVec.end(); ++it)
  {
      if (boardName == it->name)
      { //exists, so don't create
          nameExists = true;
      }
  }
  if (nameExists)
  {
      return "Error: A board with that name already exists!\n";
  }
  else
  {
      board b1(currentUser,boardName);
      boardVec.push_back(b1);
      return "Board successfully created\n";
  }
}

string serverMessage(int udp_s, vector<board> &boardVec, string currentUser, sockaddr_in &sin, sockaddr_in &sine){
    //Needed variables:
    string message, boardName, userMessage; 
    char buffy[1001];
    bool nameExists = false;
    socklen_t sinelen = sizeof(sine);
    vector<board>::iterator it;
    vector<board>::iterator it2;

    //Prompt user for the name of the board:
    message = "Which board would you like to post to? ";
    udpStrSend(udp_s, message.c_str(), &sin, sizeof(struct sockaddr), "Could not send board name request");

    //User sends the name of the board over:
    udpRecv(udp_s, buffy, 1000, &sine, &sinelen, "Did not receive board name");
    boardName = buffy;

    //Prompt user for the message that they would like to send over:
    message = "What message would you like to post on this board? ";
    udpStrSend(udp_s, message.c_str(), &sin, sizeof(struct sockaddr), "Could not send message request");

    //Receive message to be posted:
    udpRecv(udp_s, buffy, PROG4_BUFF_SIZE, &sine, &sinelen, "Did not receive user message");
    userMessage = buffy;

    //Check to see if the board exists with the given name:
    for (it = boardVec.begin(); it != boardVec.end(); ++it) {
      if (boardName == it->name) { //exists, so don't create
          nameExists = true;
      }
    }
  
    //If exists, then append the username and message to the end of the file representing the board:
    if (nameExists) {
     struct message newMess; 
     newMess.user = currentUser;
     newMess.text = userMessage;
     for(it2 = boardVec.begin(); it2 != boardVec.end(); ++it2) {
       if(boardName == it2->name) {
         (it2->messageVec).push_back(newMess);
       }
     }
     message = "The message was posted successfully.";   
     
    //Else, do not post message:
   }else{
     message = "This board does not exist."; 
   }

    //Send results back to the client:
    //udpStrSend(udp_s, message.c_str(), &sin, sizeof(struct sockaddr), "Could not send message final results");
    return message;
}

string serverDelete(int udp_s, vector<board> &boardVec, string currentUser, sockaddr_in &sin, sockaddr_in &sine){
    string message, boardName, userMessage; 
    char buffy[1001];
    socklen_t sinelen = sizeof(sine);
    int messNum;
    vector<board>::iterator it;
    
    //Prompts user for the name of the board that they would like to delete the message from:
    message = "Which board would you like to delete a message from? ";
    udpStrSend(udp_s, message.c_str(), &sin, sizeof(struct sockaddr), "Could not send board name request");

    //Receives the board name from the user:
    udpRecv(udp_s, buffy, PROG4_BUFF_SIZE, &sine, &sinelen, "Did not receive board name from user");
    boardName = buffy;

    //Asks the user what number that message is on the board:
    message = "What is the number of the message that you would like to delete on this board? ";
    udpStrSend(udp_s, message.c_str(), &sin, sizeof(struct sockaddr), "Could not send message number request"); 

    //Receives the message ID number from the user:
    udpRecv(udp_s, buffy, PROG4_BUFF_SIZE, &sine, &sinelen, "Did not receive message number from the user");
    messNum = atoi(buffy);

    //Checks to see if the board exists:
    for (it = boardVec.begin(); it != boardVec.end(); ++it) {
      if (boardName == it->name) { //board exists
          if((it->messageVec).size() >= (unsigned int)messNum) { //message exists
            if((it->messageVec).at(messNum).user == currentUser){  //username matches - then delete
              (it->messageVec).erase((it->messageVec).begin()+(messNum+1)); //Should erase it? Seems really odd...
              message = "The message was successfully deleted.\n";
 	    }else{ //username does not match
              message = "Error: Cannot delete a post that you did not post yourself.\n";
            }
          }else{ //message does not exist 
            message = "Error: The requested message does not exist.\n";
          }    
      }else{ //board does not exist
        message = "Error: The board does not exist.\n";
      }
    }
  
    //Sends the results of the previous for loop to the client:
    return message;
}

string serverEdit(int udp_s, vector<board> &boardVec, string currentUser, sockaddr_in &sin, sockaddr_in &sine){
    string message, boardName, userMessage; 
    char buffy[1001];
    socklen_t sinelen = sizeof(sine);
    int messNum;
    vector<board>::iterator it;

    //Prompts user for the name of the board that they would like to access:
    message = "Which board has the message that you would like to edit? ";
    udpStrSend(udp_s, message.c_str(), &sin, sizeof(struct sockaddr), "Could not send board name request");

    //Receives the board name from the client:
    udpRecv(udp_s, buffy, PROG4_BUFF_SIZE, &sine, &sinelen, "Did not receive board name from user");
    boardName = buffy;

    //Prompts user for the number of the message they woud like to edit:
    message = "What is the number of the message that you would like to edit? ";
    udpStrSend(udp_s, message.c_str(), &sin, sizeof(struct sockaddr), "Could not send message number request");

    //Receives the message number:
    udpRecv(udp_s, buffy, PROG4_BUFF_SIZE, &sine, &sinelen, "Did not receive message number from the user");
    messNum = atoi(buffy);

    //Prompts user for the new version of their message:
    message = "What is the new verison of this message? ";
    udpStrSend(udp_s, message.c_str(), &sin, sizeof(struct sockaddr), "Could not send edited message request");

    //Receives new message from the client:
    udpRecv(udp_s, buffy, PROG4_BUFF_SIZE, &sine, &sinelen, "Did not receive edited message from the user");
    userMessage = buffy;

    //Checks to see if board exists:
    for (it = boardVec.begin(); it != boardVec.end(); ++it) {
      if (boardName == it->name) { //board exists
        if((it->messageVec).size() >= (unsigned int)messNum) { //message exists
          if((it->messageVec).at(messNum).user == currentUser){  //username matches
            (it->messageVec).at(messNum).text = userMessage; //edits the message
            message = "Message successfully edited.\n";
          }else{ //username does not match
              message = "Error: Cannot delete a post that you did not post yourself.\n";
            }
          }else{ //message does not exist 
            message = "Error: The requested message does not exist.\n";
          }    
      }else{ //board does not exist
        message = "Error: The board does not exist.\n";
      }
    }

    //Sends results to the client:
    return message;
}

string serverList(int sock, vector<board> & boardVec,sockaddr_in & sin){
  vector<board>::iterator it;
  string listString = "";
  //loop through the boardVec to get all of the board names
  for (it = boardVec.begin(); it != boardVec.end(); ++it)
  {
    if(!it->name.empty()){
      listString = listString + it->name + "\n";
    }
  }
  if (listString.empty()){
    return "There are currently no boards.\n";
  }
  else{
    return listString;
  }
}

string serverRead(int udpSock, int tcpSock, struct sockaddr_in & sin,
                  vector<board> boardVec){
    //send prompt
    udpStrSend(udpSock, "Which board do you want to read?", 
               &sin, sizeof(struct sockaddr), "Could not send RDB prompt");
    //get response
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffy[PROG4_BUFF_SIZE];
    udpRecv(udpSock, buffy, PROG4_BUFF_SIZE, &client_addr, &addr_len, 
            "Could not receive RDB board name");

    //signal that the client needs to read a board
    udpStrSend(udpSock, "RDB", &sin, sizeof(struct sockaddr), 
               "Could not send RDB signal");
    
    //process file
    string boardName = buffy;
    int fileSize;
    int fileSizeToSend;
    vector<board>::iterator it;
    bool nameExists = false;
    for (it = boardVec.begin(); it != boardVec.end(); ++it)
    {
        if (boardName == it->name)
        { 
            nameExists = true;
            break;
        }
    }
    if (!nameExists)
    {
        //send a negative file size
        fileSize = -1;
        fileSizeToSend = htonl(fileSize);
        tcpSend(tcpSock, &fileSizeToSend, 4, 
                "Could not send negative file size");
        return "";
    }
    //create a file from the board, C style
    FILE* boardF = fopen(boardName.c_str(), "w");
    if (!boardF) 
    {
        cout << stderr << "Error: Could not open file named " << boardName 
             << endl;
        exit(1);
    }
    fprintf(boardF, "Creator: %s\n\n", it->creator.c_str());
    vector<message>::iterator msgIt;
    int i = 0;
    for (msgIt = it->messageVec.begin(); msgIt != it->messageVec.end(); ++msgIt)
    {
        fprintf(boardF, "Message %d. %s: %s\n", i, 
                msgIt->user.c_str(), msgIt->text.c_str());
        i++;
    }
    
    //send the filesize
    fflush(boardF);
    fileSize = getFileSize(boardF);
    fileSizeToSend = htonl(fileSize);
    tcpSend(tcpSock, &fileSizeToSend, 4, "Could not send file size");

    //send the filename
    tcpStrSend(tcpSock, boardName.c_str(), "RDB: Could not send filename");
    sendFile(tcpSock, boardF, fileSize, "myfrmd");

    //delete the file
    fclose(boardF);
    remove(boardName.c_str());
    
    return "";
}

string serverAppend(){
    return "";
}

string serverDownload(){
    return "";
}

string serverDestroy(int sock, string currentUser, vector<board> & boardVec, sockaddr_in & sin){
  struct sockaddr_in client_addr;
  socklen_t addr_len;
  char boardName[1000];
  addr_len = sizeof(client_addr);

  udpStrSend(sock, "Please enter the name of the board to delete:", &sin, sizeof(struct sockaddr),"Could not send request for board name");

  udpRecv(sock,boardName,1000,&client_addr,&addr_len,"myfrmd");

  //loop through boardVec to find the board to destroy
  vector<board>::iterator it;
  bool nameExists = false;
  for (it = boardVec.begin(); it != boardVec.end(); ++it)
  {
      if (boardName == it->name)
      { //the board exists, so destroy it if the current user is the creator
          if (currentUser == it->creator){
            nameExists = true;
            boardVec.erase(it);
	    it--;
          }
      }
  }
  if (nameExists)
  {
      return "Board successfully destroyed\n";
  }
  else
  {
      return "Error:Board cannot be destroyed. The board either does not exist, or you do not have permission to destroy it\n";
  }
}

string serverShutdown(int sock, vector<board> & boardVec, sockaddr_in & sin, string adminPass){
  struct sockaddr_in client_addr;
  socklen_t addr_len;
  char clientPass[1000];
  addr_len = sizeof(client_addr);

  udpStrSend(sock, "Please enter the Admin Password. WARNING: The server will be shut down and all boards destroyed:", &sin, sizeof(struct sockaddr),"Could not send request for board name");

  udpRecv(sock,clientPass,1000,&client_addr,&addr_len,"myfrmd");

  if(clientPass == adminPass){
    boardVec.clear();
    return "If this was implemented, you could shut down the server! For now, this just destroyed all boards\n";
  }
  else{
    return "Incorrect Password, you do not have permission to shut down the server\n";
  }
}

int main(int argc, char * argv[]){
  struct sockaddr_in sin;
  string message, adminPass;
  socklen_t len;
  int tcp_s, udp_s, ntcp_s, port, bytesRec;
  vector<board> boardVec;

  //check for correct number of arguments and assign arguments to variables
  if (argc==3){
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
      adminPass = argv[2];
  }
  else{
    fprintf(stderr, "usage: myftpd [Port] [Admin Password]\n");
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

  printf("Hello, and Welcome to the Server of the 12st Century!\n");

  //listen to the socket
  if((listen(tcp_s,0))<0){
      perror("myftpd:listen");
      exit(1);
  }

  map<string, string> users;
  //this loop runs every time a client connects to the server
  while (1)
  {
      //setup TCP and UDP client
      if((ntcp_s = accept(tcp_s,(struct sockaddr *)&sin,&len))<0){
          perror("myfrmd:accept");
          exit(1);
      }

      //Make the user, password map:
      struct sockaddr_in sine;
      socklen_t sinelen = sizeof(sine);
      char buffy[PROG4_BUFF_SIZE];
      bool exists = false;
      string password, username;
      socklen_t sinlen = sizeof(sin);
      string operationsMessage = "Please enter one of these codes:\nCRT: Create Board, LIS: List Boards, MSG: Leave Message, DLT: Delete Message, RDB: Read Board, EDT: Edit Message, APN: Append File, DWN: Download File, DST: Destroy Board, XIT: Exit, SHT: Shutdown Server\n";
      
      //Receive acknowledgement from client to finish UDP set-up:
      udpRecv(udp_s, buffy, PROG4_BUFF_SIZE, &sin, &sinlen, 
              "Did not receive UDP initialization");
      //Debug:
      cout << buffy << endl;
      
      /* At this point, the client is connected with TCP and UDP. */

      /* Now we ask the client to log in: */

      //Sends request for the username:
      string requestName = "Please enter your desired username: ";
      udpStrSend(udp_s, requestName.c_str(), &sin, sizeof(struct sockaddr), 
                 "Could not send username request");

      //Receives username:
      udpRecv(udp_s, buffy, PROG4_BUFF_SIZE, &sine, &sinelen, 
              "Did not receive username");
      username = buffy;

      //Checks to see if it is a new user or existing user:
      for(map<string, string>::iterator it = users.begin(); 
          it != users.end(); ++it) {
          if(it->first == username) {
              password = it->second;
              exists = true;
          }
      }

      //Requests password:
      if (exists) {
          message = "Please enter your password: ";
      }else{
          message = "Welcome! Please enter the password you would like to use: ";
      }
      udpStrSend(udp_s, message.c_str(), &sin, sizeof(struct sockaddr), 
                 "Could not send password request");

      bool wrongPass = true;
      while (wrongPass)
          {
          //Receives password:
          udpRecv(udp_s, buffy, PROG4_BUFF_SIZE, &sine, &sinelen, 
                  "Did not receive password information");
          password = buffy;

          //Checks to see if there is a new user or see if the password matches:
          if (exists) {
              if(password == users[username]) {
                  message = "The passwords matched! You have successfully logged in.\n";
                  wrongPass = false;
              }else{
                  message = "The entered password was incorrect. Please try again: ";
              }
          }else{
              users[username] = password;
              message = "Account setup has been completed. Welcome to 12st Century Forums!\n";
              wrongPass = false;
          }
          if (!wrongPass)
          {
              message += operationsMessage;
          }
          //Sends acknowledgment to the client:
          udpStrSend(udp_s, message.c_str(), &sin, sizeof(struct sockaddr), 
                     "Could not send log in acknowledgement.");
      }

      //Wait for operation from client:
      while(1){
          bytesRec =  udpRecv(udp_s, buffy, PROG4_BUFF_SIZE, &sine, &sinelen, 
                              "Could not receive client opcode");
          if(bytesRec==0) break; //client ^C
          if(strcmp("CRT",buffy)==0){
              message = serverCreate(udp_s, username, boardVec, sin);
          }
          else if(strcmp("MSG",buffy)==0){
              message = serverMessage(udp_s, boardVec, username, sin, sine);
          }
          else if(strcmp("DLT",buffy)==0){
              message = serverDelete(udp_s,boardVec, username, sin, sine);
          }
          else if(strcmp("EDT",buffy)==0){
              message = serverEdit(udp_s,boardVec, username, sin, sine);
          }
          else if(strcmp("LIS",buffy)==0){
              message = serverList(udp_s, boardVec, sin);
          }
          else if(strcmp("RDB",buffy)==0){
              //serverRead(udp_s);
          }
          else if(strcmp("APN",buffy)==0){
              //serverAppend(ntcp_s);
          }
          else if(strcmp("DWN",buffy)==0){
              //serverDownload(ntcp_s);
          }
          else if(strcmp("DST",buffy)==0){
              message = serverDestroy(udp_s, username, boardVec, sin);
          }
          else if(strcmp("XIT",buffy)==0){
              udpStrSend(udp_s, "XIT", &sin, sizeof(struct sockaddr), 
                         "Could not send log in acknowledgement.");
              break; //exit inner while loop
          }
          else if(strcmp("SHT",buffy)==0){
              message = serverShutdown(udp_s, boardVec, sin, adminPass);
          }
          else{
              //strcpy(message,"Send a correct command\n");
          }
          printf("Received:%s\n",buffy);
          message += operationsMessage;
          udpStrSend(udp_s, message.c_str(), &sin, sizeof(struct sockaddr), 
                     "Could not send action response message");
      }
      printf("Client Quit!\n");
      close(ntcp_s);
  }
  close(udp_s);
  close(tcp_s);
}
