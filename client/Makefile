#  Authors: Jacob Kassman, Lauren Kuta, Matt Paulson
#  netIDs: jkassman, lkuta, mpaulson
#  Computer Networks: Programming Assignment 4
#  Makefile 
#  Makefile for our client code.

myfrm: myfrm.cpp common.o
	g++ -Wall myfrm.cpp common.o -o myfrm -lmhash
	
common.o: ../common.cpp ../common.hpp
	g++ -c ../common.cpp -Wall -lmhash
clean:
	rm myfrm common.o
