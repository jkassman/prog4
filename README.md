Authors: Jacob Kassman, Lauren Kuta, Matt Paulson
netIDs: jkassman, lkuta, mpaulson
Computer Networks: Programming Assignment 4
README.md
Date: 11-13-2016

List of files included in the project:

Client: client/myfrm.cpp
  -The C++ source code for the client.
  -The client will initiate CRT, LIS, MSG, DLT, RDB, EDT, APN, DWN, DST, XIT, and SHT commands.
  -The client displays information to the user when appropriate.
  -Run this program with the following command: ./myfrm student01.cse.nd.edu 41031

Server: server/myfrmd.cpp
  -The C++ source code of the server.
  -The server will process and execute the CRT, LIS, MSG, DLT, RDB, EDT, APN, DWN, DST, XIT, and SHT commands.
  -The server never prints anything to the screen, except in case of error.
  -Run this program with the following command: ./myfrmd 41031 admin_password
      -admin_password can be anything that the server admin chooses.

Makefiles:
  -client/Makefile (for client side)
  -server/Makefile (for server side)

Other files:
  -common.cpp, common.hpp
     -library that contains functions that both client and server use.

  -server/board.cpp, server/board.hpp
     -Contains the definitions for a board class used to store and set up the user-made message board.
     -Also includes definitions for message and file structs,
      which store the user-made messages, the user-appended files, and other necessary information.


