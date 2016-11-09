/*
  Authors: Jacob Kassman, Lauren Kuta, Matt Paulson
  netIDs: jkassman, lkuta, mpaulson
  Computer Networks: Programming Assignment 4
  common.hpp
  Defines several functions used frequently throughout both our server and client code.  
 */

#ifndef PROG4_COMMON_HPP
#define PROG4_COMMON_HPP

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <mhash.h>
#include <dirent.h>
#include <sys/time.h>

#define PROG3_BUFF_SIZE 4096

int udpSend(int sock, void *data, size_t data_len, 
            struct sockaddr_in *dest_addr, socklen_t dest_len, 
            const char *errorMsg);
int udpStrSend(int sock, const char *stringToSend, 
               struct sockaddr_in *dest_addr, 
               socklen_t dest_len, const char *errorMsg);
int udpRecv(int sock, void *data, size_t data_len, 
            struct sockaddr_in *address, 
            socklen_t *address_len, const char *errorMsg);

int tcpRecv(int sock, void *data, size_t data_len, const char *errorMsg);
int tcpSend(int sock, void *data, size_t data_len, const char *errorMsg);
int tcpStrSend(int sock, const char *stringToSend, const char *errorMsg);
int stringToInt(int *destInt,char *toConvert, int minVal, int maxVal);
void hashFile(unsigned char hash[], FILE* fileToHash);
int hashCompare(unsigned char *hash1, unsigned char *hash2);
void sendFile(int sock, FILE* file, unsigned int fileSize, 
              const char *programName);
void recvFile(int sock, FILE *f, unsigned int fileSize,
              const char *programName);
unsigned int getFileSize(FILE *f);
int getNameFromUser(char *name, int maxNameLen);
int min(int x, int y);
#endif
