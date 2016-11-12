/*
  Authors: Jacob Kassman, Lauren Kuta, Matt Paulson
  netIDs: jkassman, lkuta, mpaulson
  Computer Networks: Programming Assignment 4
  common.hpp
  Defines the class that is used to store message boards
 */

#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>

class board{
  struct message{
    string user;
    string text;
  }
  vector<message> messageVec;
  string name;
  string creator;

  public:
    board(string user, string name);
//messageVec
//message struct each message has user and text
//create function, pass user and name
};

#endif
