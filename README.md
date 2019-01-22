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



WINSOCK:

#include "flowerMeadow.h"

int main()
{
	initialConnectToServer("192.168.33.1", "5555", "Yannis");
	//connectToServer("192.168.33.1", "5555");
	spawnPlayer();
	startMowing();
	rotatePlayer(100);
	movePlayer(20);
	printf("Distance to wall ahead: %lf\n", getDistToWall());
	disconnect();

    return 0;
}
