#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdint.h>

#define DEFAULT_BUFLEN 512

void __cdecl sendMessage(char *sendbuf);

void initialConnectToServer(char* ipAddr, char* portNum, char username[]);

void connectToServer(char* ipAddr, char* portNum);

void disconnect();

void spawnPlayer();

void deletePlayer();

void rotatePlayer(double angle);

void movePlayer(int speed);

void shoot();

void startDrawing(char color[]);

void stopAndClearDrawing();

void startMowing();

void stopMowing();

char* getStatus();

double getDistToWall();

char* getPlayersInRadius(int radius);

char* getPickUpsInRadius(int radius);

char* getDirectionVector();

char* getAngleWall();

char* getTipp();

char* getWallDistance();
