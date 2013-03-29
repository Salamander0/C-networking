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

#define MAX 50

const char *HELPFILE =
"*\n"
"* Program:  client.c\n"
"* Datum:   22.3.2012\n"
"* Author:   Salamander\n"
"* Usage:\n"
"* client -h HOSTNAME -p PORT -l USER(s) -u UID(s) -L -U -G -N -H -S\n"
"* -h       set hostname adress, mandatory!\n"
"* -p       set connection port, mandatory!\n"
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
    
    if(port <= 0){
        printf("%s", HELPFILE);
        return EXIT_FAILURE;
    }
    
    if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
        perror("error on socket");
        return EXIT_FAILURE;
    }
    
    sin.sin_family = PF_INET;
    sin.sin_port = htons(port);
    
    if((hptr =  gethostbyname(server) ) == NULL){
        fprintf(stderr, "gethostname error: %s", server);
        return EXIT_FAILURE;
    
    }
    memcpy(&sin.sin_addr, hptr->h_addr, hptr->h_length);
    if(connect (s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 ){
        perror("error on connect");
        return EXIT_FAILURE;
    }
    
    //odeslani adresy domeny na server
    if(write(s, params, strlen(params) +1) < 0 ) {
        perror("error on write");
        return EXIT_FAILURE;
    }
    
    //prijem zpravy ze serveru
    if((n = (int)read(s, recvd, sizeof(recvd) ) ) <0) {  /* read message from server */
        perror("error on read");
        return EXIT_FAILURE;
    }
    
    printf ("%s", recvd);  /* print message to screen */
    
	//uzavreni spojeni
    if(close(s) < 0) {
        perror("error on close");
        return EXIT_FAILURE;
    }
    return 0;
}

int main (int argc, char **argv)
{
    char *server, str[20]="", params[100]="", *optstring = "h:p:l:u:LUGNHS", *login[MAX], *next;
    int port=0, c, index;
    extern int optind, optopt;
    unsigned int uid[MAX];
    size_t lcount=0, ucount=0, pflag=0, cflag=0;
    
	//argumenty prikazove radky
	if (argc < 3){
		fprintf(stdout,"%s\n",HELPFILE);
		return EXIT_FAILURE;
	}
    
    while((c = getopt(argc, argv, optstring)) != -1){
        switch(c){
            case 'h':
                server = strdup(optarg);
                if(server == NULL){
                    fprintf(stderr, "Server string mismatch");
                    exit(EXIT_FAILURE);
                }
                else cflag++;
                break;
            case 'p':
                port = atoi(optarg);
                if(port <= 0){
                    fprintf(stderr, "Port number mismatch");
                    exit(EXIT_FAILURE);
                }
                else pflag++;
                break;
            case 'l':
                index = optind-1;
                while(index < argc){
                    next = strdup(argv[index]);    /* get login */
                    index++;
                    if(next[0] != '-'){             /* check if optarg isn't next switch */
                        login[lcount++] = next;
                    }
                    else break;
                }
                optind = index-1;
                break;
            case 'u':
                index = optind-1;
                while(index < argc){
                    next = strdup(argv[index]);    /* get uid */
                    index++;
                    if(next[0] != '-'){             /* check if optarg isn't next switch */
                        uid[ucount++] = atoi(next);
                    }
                    else break;
                }
                optind = index-1;                           /* set optind to correct value */
                break;
            case 'L':
                strcat(str, " -L");                            /* get name from passwd */
                break;
            case 'U':
                strcat(str, " -U");                            /* get uid from passwd */
                break;
            case 'G':
                strcat(str, " -G");                            /* get gid from passwd */
                break;
            case 'N':
                strcat(str, " -N");                            /* get full name, gecos */
                break;
            case 'H':
                strcat(str, " -H");                            /* get home folder */
                break;
            case 'S':
                strcat(str, " -S");                            /* get default shell */
                break;
            case ':':                               /* error - missing operand */
                fprintf(stderr, "Option -%c requires an operand\n", optopt);
                break;
            case '?':                               /* error - unknown option */
                fprintf(stderr,"Unrecognized option: -%c\n", optopt);
                break;
        }
    }
    
    if((pflag == 0)||(cflag == 0)){
        fprintf(stderr,"-h HOSTNAME and -p PORT are mandatory parameters!\n");
        return EXIT_FAILURE;
    }
    
    if((lcount == 0) && (ucount == 0)){
        fprintf(stderr, "No UID or users entered.\n");
        return EXIT_FAILURE;
    }

    
    if(lcount > 0){
        for(unsigned int i=0; i<lcount; i++){
            strcpy(params, "-l ");
            strcat(params, login[i]);
            strcat(params, str);
            connection(port, server, params);
        }
    }
    
    if(ucount > 0){
        for(unsigned int i=0; i<ucount; i++){
            strcpy(params, "-u ");
            char suid[5];
            sprintf(suid, "%d", uid[i]);
            strcat(params, suid);
            strcat(params, str);
            connection(port, server, params);
        }
    }
	return EXIT_SUCCESS;
}