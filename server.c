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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


const char *HELPFILE =
"*\n"
"* Program:  server.c\n"
"* Date:   19.3.2012\n"
"* Author:  Salamander\n"
"* Usage:\n"
"* server -p PORT\n"
"*\n";

void SigCatcher(int param){
    int status;
    struct rusage usage;
	wait3(&status, WNOHANG, &usage);
}	

int main(int argc, char * argv[])
{
    int c, rc, port=0;
    int welcome_socket;                             /* socket used to listen for incoming connections */
    char *optstring = "p:", MSG[100] = "", command[1024] = "";
    struct sockaddr_in6 sa;                         /* socket info about our server */
    struct sockaddr_in6 sa_client;                  /* socket info about client connecting to serv */
    char str[INET6_ADDRSTRLEN];
    extern int optopt, optind;
    extern char *optarg;
    
    
    /* command line arguments */
    if(argc < 3){
        fprintf(stderr,"%s", HELPFILE);
    }
    
    while((c = getopt(argc, argv, optstring)) != EOF){
        switch(c){
            case 'p':
                port = atoi(optarg);
                break;
            case ':':
                if(optopt == 'p'){
                    fprintf(stderr,"Option -%c requires an argument\n", optopt);
                    return EXIT_FAILURE;
                }
                else if(isprint(optopt)){
                    fprintf(stderr,"Unknown option '-%c'\n", optopt);
                    return EXIT_FAILURE;
                }
                else{
                    fprintf(stderr, "Unknown option character '\\x%x'\n", optopt);
                    return EXIT_FAILURE;
                }
        }
    }
    
    if(port <= 0){
        printf("%s", HELPFILE);  /* wrong port number */
        return EXIT_FAILURE;
    }
    
    if (getcwd(command, sizeof(command)) == NULL) perror("getcwd() error"); /* get current working directory for popen to work correctly */
    strcat(command, "/getpwd ");

    socklen_t sa_client_len=sizeof(sa_client);
    if((welcome_socket = socket(PF_INET6, SOCK_STREAM, 0)) < 0){
        perror("socket() failed");
        return EXIT_FAILURE;
    }
    
    memset(&sa,0,sizeof(sa));                               /* zero the struct before filling in the fields */
    sa.sin6_family = AF_INET6;                              /* set type of connection to TCP/IPv6 */
    sa.sin6_addr = in6addr_any;                             /* set address to any interface */
    sa.sin6_port = htons(port);                             /* set the server port number */
    
    /* bind serv information to socket */
    if((rc = bind(welcome_socket, (struct sockaddr*)&sa, sizeof(sa))) < 0){
        perror("bind() failed");
        return EXIT_FAILURE;
    }
    
    /* start listening, allowing a queue of up to 1 pending connection */
    if ((rc = listen(welcome_socket, 1)) < 0){
		perror("listen() failed");
		return EXIT_FAILURE;
	}
    
    signal(SIGCHLD, SigCatcher);        /* catch child process */
    
    while(1){
        int comm_socket = accept(welcome_socket, (struct sockaddr*)&sa_client, &sa_client_len);     /* accept connections */
        if (comm_socket > 0){
            int pid = fork();
            if(pid < 0){
                perror("fork() failed");
                return EXIT_FAILURE;
            }
            /* new process to handle clients requests */
            if(pid == 0){                                                   
                close(welcome_socket);                                      /* not needed anymore */
                int flags = fcntl(comm_socket, F_GETFL, 0);
                rc = fcntl(comm_socket, F_SETFL, flags | O_NONBLOCK);       /* set to non-block (not really needed) */
                if (rc < 0){
                    perror("fcntl() failed");
                    exit(EXIT_FAILURE);								
                }
                
                if(inet_ntop(AF_INET6, &sa_client.sin6_addr, str, sizeof(str))){}
                if(recv(comm_socket, MSG, sizeof(MSG), 0) <0){              /* read message from client */
                    perror("error on read");
                    return EXIT_FAILURE;
                }
                
                    strcat(command, MSG);
                    FILE *fp;
                    fp = popen(command, "r");                               /* call the getpwd utility */
                    if (fp == NULL) {
                        printf("Failed to run command\n" );
                        return EXIT_FAILURE;
                    }
                    fgets(MSG, sizeof(MSG), fp);                            /* read standard output to MSG (100 characters limit) */
                    pclose(fp);
                
                    /* send mesasage back to client */
                    if(send(comm_socket, MSG, sizeof(MSG), 0) <0){
                        perror("error on write");
                        return EXIT_FAILURE;
                    }
                
                close(comm_socket);
                exit(EXIT_SUCCESS);
            }
            else close(comm_socket);
        }
    }
    return EXIT_SUCCESS;
}

