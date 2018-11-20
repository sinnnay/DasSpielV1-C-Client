#include "dasSpiel.h"

/*
 * Distanz zur Wand, welche sich in Bewegungsrichtung befindet
 * und als Rückgabewert dient
 */
static double distToWall;

/*
 * Verbindungsdaten zum Unity-Server - bestehend aus
 * IP-Adresse und Port-Nummer
 */
static char *ip, *port;

/*
 * Funktion, welche in der Bibliothek zum Zusammenfügen
 * von Nachrichten benötigt wird.
 * Diese werden an den Server gesendet
 */
static char *concatStrings(char* str1, char* str2){
	char *finalMsgStr = malloc(80);
    strcpy(finalMsgStr, str1);
	strcat(finalMsgStr, str2);
	return finalMsgStr;
}

/*
 * sendMessage kommuniziert über TCP mit dem Server,
 * hierzu wird bei jedem aufruf ein neuer Socket erstellt
 */
void __cdecl sendMessage(char *sendbuf)
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialisierung von WinSock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup fehlgeschlagen mit diesem Fehler: %d\n", iResult);
        return;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    /*
     * Für den Fall, dass nur der Servername übergeben wurde,
     * wird nun hier die zugehörige IP-Adresse und Port-Nummer ermittelt
     */
    iResult = getaddrinfo(ip, port, &hints, &result);
    if ( iResult != 0 ) {
        printf("Fehler bei Erlangung der Verbindungs-Informationen: %d\n", iResult);
        WSACleanup();
        return;
    }

    /*
     * Solange versuchen die Verbindung zum Server aufzubauen,
     * bis es klappt
     */
    for(ptr = result; ptr != NULL; ptr=ptr->ai_next) {
        // Erstellung des Sockets:
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("Aufbau der Verbindung fehlgeschlagen: %d\n", WSAGetLastError());
            WSACleanup();
            return;
        }

        // Verbindung zum Server
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Verbindungsaufbau fehlgeschlagen!\n");
        WSACleanup();
        return;
    }

    // Nachricht an den Server versenden
    iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );

    if (iResult == SOCKET_ERROR) {
        printf("Versand der Nachticht an den Server fehlgeschlagen: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    /*
     * Für den Fall, dass eine Antwort des Servers zu
     * erwarten ist:
     */
    if (strstr(sendbuf, "STATUS;") != NULL || strstr(sendbuf, "DIST;") != NULL) {
		// Solange Empfangen bis alles angekommen ist
		do {
			iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			if ( iResult > 0 ){
				if (strstr(sendbuf, "DIST;") == NULL){
					printf("%s\n", recvbuf);
					fflush(stdout);
				}
			}
			else if ( iResult == 0 )
				printf("Verbindung wurde geschlossen\n");
			else
				printf("Empfangen der Daten vom Server fehlgeschlagen: %d\n", WSAGetLastError());
		} while(strstr(recvbuf, ";") == NULL);

		if (strstr(sendbuf, "DIST;") != NULL) {
			sscanf(recvbuf, "%lf", &distToWall);
		}
    }


    // Herunterfahren der Verbindung
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("Verbindung konnte nicht erfolgreich abgeschlossen werden: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    // Schließen des Sockets
    closesocket(ConnectSocket);
    WSACleanup();

}

/*
 * Im Anschluss folgen die verfügbaren Operationen,
 * auf die der Server reagieren kann
 */

void connectToServer(char* ipAddr, char* portNum, char username[]) {
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
	char str2[] = "";
	str1 = "ROTATE|";
	sprintf(str2, "%d", angle);
	str3 = ";";
	strFinal = concatStrings(str1, str2);
	strFinal = concatStrings(strFinal, str3);
	sendMessage(strFinal);
}

void movePlayer(int speed) {
	char *str1, *str3, *strFinal;
	char str2[] = "";
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

