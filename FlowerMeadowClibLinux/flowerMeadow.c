/* 
 * flowerMeadow.c - Erlaubt es mit dem Unity-Server zu kommunizieren, 
 * und implementiert die Funktionen, die das Spielen des Spiels ermöglichen 
 */

#include "flowerMeadow.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static char *ip, *messageFromServer;
static int port;
static double distToWall;
static int64_t lastTimeMessageSentMs, timePassedSinceLastMsgSentMs, currentTimeMs;

int64_t currentTimeMillis()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    int64_t s1 = (int64_t) (time.tv_sec) * 1000;
    int64_t s2 = (time.tv_usec / 1000);
    return s1 + s2;
}

/* 
 * Wrapper fuer perror
 */
void error(char *msg)
{
    perror(msg);
    exit(0);
}

/* 
 * Funktion zum Zusammenfügen von Zeichenketten, 
 * welche eine Nachricht an den Server ergeben sollen
 */
static char *concatStrings(char *str1, char *str2)
{
    char *finalMsgStr = malloc(80);
    strcpy(finalMsgStr, str1);
    strcat(finalMsgStr, str2);
    return finalMsgStr;
}

void sendMessage(char *sendbuf)
{
    pthread_mutex_lock(&lock);
    currentTimeMs = currentTimeMillis();
    timePassedSinceLastMsgSentMs = currentTimeMs - lastTimeMessageSentMs;
    if(timePassedSinceLastMsgSentMs < 500){
        usleep((500 - timePassedSinceLastMsgSentMs) * 1000);
    }
    int sockfd, iResult;
    struct sockaddr_in serveraddr;
    struct hostent *server;

    char recvbuf[BUFSIZE];

    /* Eroeffnung des Sockets */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("Fehler bei Eroeffnung des Sockets");

    /* gethostbyname: DNS-Eintrag vom Server bekommen */
    server = gethostbyname(ip);
    if (server == NULL)
    {
        fprintf(stderr, "Fehler, IP-Adresse %s wurde nicht gefunden\n", ip);
        exit(0);
    }

    /* Internet-Adresse vom Server bauen */
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(port);

    /* connect: Aufbauen einer Verbindung zum Server */
    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
        error("Fehler beim Verbinden");

    /* write: Versenden einer Nachricht an den Server */
    iResult = write(sockfd, sendbuf, strlen(sendbuf));
    if (iResult < 0)
        error("Fehler beim Schreiben zum Socket");

    /* 
     * Die Nachricht vom Client an den Server beinhaltet eine Anfrage, 
     * welche eine Antwort vom Server nach sich zieht
     */
    struct timeval tv = {3, 0};  // 3 Sekunden Timeout
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));

    if (strstr(sendbuf, "STATUS;") != NULL || strstr(sendbuf, "DIST;") != NULL || strstr(sendbuf, "DIR_VECT;") != NULL
		|| strstr(sendbuf, "SURR_PLAYERS") != NULL || strstr(sendbuf, "SURR_PICKUPS") != NULL || strstr(sendbuf, "CONNECT") != NULL
		|| strstr(sendbuf, "WALL;") != NULL || strstr(sendbuf, "ANGLE;") != NULL || strstr(sendbuf, "TIPP;") != NULL)   
    {
        // Solange Bytes vom Server Empfangen, bis die Antwort zu ende ist
        do
        {
            bzero(recvbuf, BUFSIZE);
            iResult = read(sockfd, recvbuf, BUFSIZE);
            if (iResult == 0)
	    {
                printf("Verbindung geschlossen\n");
		return;
	    }
            else if(iResult < 0)
	    {
                if (errno == EWOULDBLOCK || errno == EAGAIN)
	        {
		    printf("Server antwortet nicht\n");
	        }
		else
		    printf("Fehler beim Lesen vom Socket\n");
		return;
  	    }
        } while (strstr(recvbuf, ";") == NULL);

        if (strstr(sendbuf, "DIST;") != NULL)
        {
            sscanf(recvbuf, "%lf", &distToWall);
        }
	else if (strstr(sendbuf, "CONNECT") != NULL)
        {
            printf("%s\n", recvbuf);
            fflush(stdout);
        }
	else
        {
            messageFromServer = recvbuf;
        }
    }
    close(sockfd);
    lastTimeMessageSentMs = currentTimeMillis();
    pthread_mutex_unlock(&lock);
}


void initialConnectToServer(char* ipAddr, int portNum, char username[]) {
	ip = ipAddr, port = portNum;
	char *str1, *str3, *strFinal;
	str1 = "CONNECT|";
	str3 = ";";
	strFinal = concatStrings(str1, username);
	strFinal = concatStrings(strFinal, str3);
	sendMessage(strFinal);
	free(strFinal);
}

void connectToServer(char* ipAddr, int portNum) {
	ip = ipAddr, port = portNum;
}

void disconnect() {
	sendMessage("DISCONNECT;");
}

void spawnPlayer() {
	sendMessage("SPAWN;");
}

void deletePlayer() {
	sendMessage("DELETE;");
}

void rotatePlayer(double angle) {
	char *str1 = {'\0'}, *str3 = {'\0'}, *strFinal = {'\0'};
	char str2[316];
	str1 = "ROTATE|";
	sprintf(str2, "%.4lf", angle);
	str3 = ";";
	strFinal = concatStrings(str1, str2);
	strFinal = concatStrings(strFinal, str3);
	sendMessage(strFinal);
}

void movePlayer(int speed) {
	if(speed > 500 || speed < -500) {
		printf("Speed muss im Bereich von -500 und 500 sein\n");
		return;
	}
	char *str1, *str3, *strFinal;
	char str2[12];
	str1 = "MOVE|";
	sprintf(str2, "%d", speed);
	str3 = ";";
	strFinal = concatStrings(str1, str2);
	strFinal = concatStrings(strFinal, str3);
	sendMessage(strFinal);
}

void shoot() {
	sendMessage("SHOOT;");
}

void startDrawing(char color[]) {
	char *str1, *str3, *strFinal;
	str1 = "DRAW|";
	str3 = ";";
	strFinal = concatStrings(str1, color);
	strFinal = concatStrings(strFinal, str3);
	sendMessage(strFinal);
}

void stopAndClearDrawing() {
	sendMessage("STOP_DRAWING;");
}

void startMowing() {
	sendMessage("MOW;");
}

void stopMowing() {
	sendMessage("STOP_MOWING;");
}

char* getStatus() {
	sendMessage("STATUS;");
	return messageFromServer;
}

double getDistToWall() {
	sendMessage("DIST;");
	return distToWall;
}

char* getPlayersInRadius(int radius) {
	if(radius > 250 || radius < 0) {
		printf("Radius muss im Bereich von 0 und 250 sein\n");
		return "";
	}
	char *str1, *str3, *strFinal;
	char str2[12];
	str1 = "SURR_PLAYERS|";
	sprintf(str2, "%d", radius);
	str3 = ";";
	strFinal = concatStrings(str1, str2);
	strFinal = concatStrings(strFinal, str3);
	sendMessage(strFinal);
	return messageFromServer;
}

char* getPickUpsInRadius(int radius) {
	if(radius > 250 || radius < 0) {
		printf("Radius muss im Bereich von 0 und 250 sein\n");
		return "";
	}
	char *str1, *str3, *strFinal;
	char str2[12];
	str1 = "SURR_PICKUPS|";
	sprintf(str2, "%d", radius);
	str3 = ";";
	strFinal = concatStrings(str1, str2);
	strFinal = concatStrings(strFinal, str3);
	sendMessage(strFinal);
	return messageFromServer;
}

char* getDirectionVector() {
	sendMessage("DIR_VECTOR;");
	return messageFromServer;
}

char* getAngleWall() {
	sendMessage("ANGLE;");
	return messageFromServer;
}
	
char* getTipp() {
	sendMessage("TIPP;");
	return messageFromServer;
}

char* getWallDistance() {
	sendMessage("WALL;");
	return messageFromServer;
}
