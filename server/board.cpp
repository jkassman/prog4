/*
  Authors: Jacob Kassman, Lauren Kuta, Matt Paulson
  netIDs: jkassman, lkuta, mpaulson
  Computer Networks: Programming Assignment 4
  common.hpp
  Defines the class that is used to store message boards
 */

#include "board.hpp"

board::board(string u,string n){
  this->creator = u;
  this->name = n;
}
