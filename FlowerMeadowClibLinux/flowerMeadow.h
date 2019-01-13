#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#define BUFSIZE 1024

void sendMessage(char *sendbuf);

void initialConnectToServer(char* ipAddr, int portNum, char username[]);

void connectToServer(char* ipAddr, int portNum);

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
