#  Authors: Jacob Kassman, Lauren Kuta, Matt Paulson
#  netIDs: jkassman, lkuta, mpaulson
#  Computer Networks: Programming Assignment 4
#  Makefile 
#  Makefile for our server code.

myfrmd: myfrmd.cpp common.o board.o
	g++ -Wall myfrmd.cpp common.o board.o -o myfrmd -lmhash

board.o: board.cpp board.hpp
	g++ -c board.cpp -Wall

common.o: ../common.cpp ../common.hpp
	g++ -c ../common.cpp -Wall -lmhash

clean:
	rm myfrmd common.o board.o
