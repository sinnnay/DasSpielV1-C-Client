/* 
 * dasSpiel.c - Erlaubt es mit dem Unity-Server zu kommunizieren, 
 * und implementiert die Funktionen, die das Spielen des Spiels ermöglichen 
 */

#include "dasSpiel.h"

static char *ip;
static int port;
static double distToWall;

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
    if (strstr(sendbuf, "STATUS;") != NULL || strstr(sendbuf, "DIST;") != NULL)   
    {
        // Solange Bytes vom Server Empfangen, bis die Antwort zu ende ist
        do
        {
            bzero(recvbuf, BUFSIZE);
            iResult = read(sockfd, recvbuf, BUFSIZE);
            if (iResult > 0)
            {
                if (strstr(sendbuf, "DIST;") == NULL)
                {
                    printf("%s\n", recvbuf);
                    fflush(stdout);
                }
            }
            else if (iResult == 0)
                printf("Verbindung geschlossen\n");
            else
                error("Fehler beim Lesen vom Socket");
        } while (strstr(recvbuf, ";") == NULL);

        if (strstr(sendbuf, "DIST;") != NULL)
        {
            sscanf(recvbuf, "%lf", &distToWall);
        }
    }
    close(sockfd);
}

void connectToServer(char* ipAddr, int portNum, char username[]) {
	ip = ipAddr, port = portNum;
    char *str1, *str3, *strFinal;
    str1 = "CONNECT|";
    str3 = ";";
    strFinal = concatStrings(str1, username);
    strFinal = concatStrings(strFinal, str3);
	sendMessage(strFinal);
	free(strFinal);
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

void rotatePlayer(int angle) {
	char *str1, *str3, *strFinal;
	char str2[12];
	str1 = "ROTATE|";
	sprintf(str2, "%d", angle);
	str3 = ";";
	strFinal = concatStrings(str1, str2);
	strFinal = concatStrings(strFinal, str3);
	sendMessage(strFinal);
}

void movePlayer(int speed) {
	char *str1, *str3, *strFinal;
	char str2[12];
	str1 = "MOVE|";
	sprintf(str2, "%d", speed);
	str3 = ";";
	strFinal = concatStrings(str1, str2);
	strFinal = concatStrings(strFinal, str3);
	sendMessage(strFinal);
}

void shootBullet() {
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

void getStatus() {
	sendMessage("STATUS;");
}

double getDistToWall() {
	sendMessage("DIST;");
	return distToWall;
}

