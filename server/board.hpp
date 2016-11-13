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

using namespace std;

struct message{
  string user;
  string text;
};

class board{
  public:
    vector<message> messageVec;
    vector<string> fileVec;
    string name;
    string creator;

    board(string user, string name);
};

#endif
