#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 1024

void sendMessage(char *sendbuf);

void connectToServer(char* ipAddr, int portNum, char username[]);

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
