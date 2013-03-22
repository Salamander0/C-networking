/*
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

const char *HELPFILE =
"*\n"
"* Program:  client.c\n"
"* Datum:   22.3.2012\n"
"* Author:   Salamander\n"
"* Usage:\n"
"* client HOST:PORT -l USER(s) -u UID(s) -L -U -G -N -H -S\n"
"* -L       return username\n"
"* -U       return UID\n"
"* -G       return GID\n"
"* -N       return full name, whole gecos\n"
"* -H       return home folder\n"
"* -S       return login shell\n"
"*\n";

int connection(int port, char *server, char *params)
{
    int s, n;
    char recvd[100]="";
    struct sockaddr_in sin; struct hostent *hptr;
    
    if ((s = socket(PF_INET, SOCK_STREAM, 0 ) ) < 0){
        perror("error on socket");
        return EXIT_FAILURE;
    }
    
    sin.sin_family = PF_INET;
    sin.sin_port = htons(port);
    
    if ( (hptr =  gethostbyname(server) ) == NULL){
        fprintf(stderr, "gethostname error: %s", server);
        return EXIT_FAILURE;
    
    }
    memcpy( &sin.sin_addr, hptr->h_addr, hptr->h_length);
    if (connect (s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 ){
        perror("error on connect");
        return EXIT_FAILURE;
    }
    
    //odeslani adresy domeny na server
    if ( write(s, params, strlen(params) +1) < 0 ) {
        perror("error on write");
        return EXIT_FAILURE;
    }
    
    //prijem zpravy ze serveru
    if (( n = read(s, recvd, sizeof(recvd) ) ) <0) {  /* read message from server */
        perror("error on read");
        return EXIT_FAILURE;
    }
    
    printf ("\n%s\n", recvd);  /* print message to screen */
    
	//uzavreni spojeni
    if (close(s) < 0) {
        perror("error on close");
        return EXIT_FAILURE;
    }
    return 0;
}

int main (int argc, char **argv)
{
    char *server, *str, params[100]="";
    int port = 0;
    
	//argumenty prikazove radky
	if (argc < 3){
		fprintf(stdout,"%s\n",HELPFILE);
		return EXIT_FAILURE;
	}
    
    //rozdeleni server:port
	str = strtok (argv[1],":");
    if(str != NULL)server = str;
    else{
        perror("server string mismach");
        return EXIT_FAILURE;
    }
    str = strtok (NULL, ":");
	if(str != NULL)port = atoi(str);
    else{
        perror("server number mismach");
        return EXIT_FAILURE;
    }
    
	strcpy(params, "-l salamander unknown new admin -L -U -G -N -H -S");
    
	connection(port, server, params);
	return EXIT_SUCCESS;
}