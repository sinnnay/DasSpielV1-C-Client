#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_BUFLEN 512

void __cdecl sendMessage(char *sendbuf);

void connectToServer(char* ipAddr, char* portNum, char username[]);

void disconnect();

void spawnPlayer();

void deletePlayer();

void rotatePlayer(int angle);

void movePlayer(int speed);

void shootBullet();

void startDrawing(char color[]);

void stopAndClearDrawing();

void getStatus();

double getDistToWall();

