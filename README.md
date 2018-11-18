# DasSpielV1-C-Client

// Beispielanwendung: play.c

#include "dasSpiel.h"

int main()
{
	connectToServer("192.168.178.51", 41703, "Yannis");  // connectToServer muss bei jeder Programmausführung aufgerufen werden 
	spawnPlayer();
	getStatus();
	movePlayer(5);
  rotatePlayer(90);
	printf("%lf\n", getDistToWall());

  return 0;
}

Compilation: gcc -Wall play.c dasSpiel.c -o play
Ausführung: ./play
