#  Authors: Jacob Kassman, Lauren Kuta, Matt Paulson
#  netIDs: jkassman, lkuta, mpaulson
#  Computer Networks: Programming Assignment 3
#  Makefile 
#  Makefile for our server code.

myfrmd: myfrmd.cpp common.o
	g++ -Wall myfrmd.cpp common.o -o myfrmd -lmhash

common.o: ../common.cpp ../common.hpp
	g++ -c ../common.cpp ../common.hpp -Wall -lmhash

clean:
	rm myfrmd common.o
