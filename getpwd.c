#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX 50                                      /* maxiumum number of login and uid parameters */

const char *HELPFILE =
"*\n"
"* Program:  getpwd.c\n"
"* Date:   17.3.2012\n"
"* Author:   \n"
"* Usage:\n"
"* getpwd -l USER(s) -u UID(s) -L -U -G -N -H -S\n"
"* -L       return username\n"
"* -U       return UID\n"
"* -G       return GID\n"
"* -N       return full name, whole gecos\n"
"* -H       return home folder\n"
"* -S       return login shell\n"
"*\n";


int main(int argc, char *argv[])
{
    struct passwd pwd;
    struct passwd *result;
    char *buf, *opts = "l:u:LUGNHS";
    size_t bufsize;
    int s, c, index;
    size_t nflag=0,uflag=0,gflag=0,aflag=0,hflag=0,sflag=0,lcount=0,ucount=0;
    extern int optopt, optind;
    char *login[MAX], *uid[MAX], *next;
    
    if (argc < 3){
		fprintf(stderr,"%s\n",HELPFILE);
		exit(EXIT_FAILURE);
	}
    
    while((c = getopt(argc, argv, opts)) != EOF){
        switch(c){
            case 'l':
                index = optind-1;
                while(index < argc){
                    next = strdup(argv[index]);    /* get login */
                    index++;
                    if(next[0] == '-'){             /* check if optarg isn't next switch */
                        break;
                    }
                    else{
                        login[lcount++] = next;
                        //printf("login: %s\n", login[lcount-1]);
                    }
                }
                break;
            case 'u':
                index = optind-1;
                while(index < argc){
                    next = strdup(argv[index]);    /* get uid */
                    index++;
                    if(next[0] == '-'){             /* check if optarg isn't next switch */
                        break;
                    }
                    else{
                        uid[ucount++] = next;
                        //printf("uid: %s\n", uid[ucount-1]);
                    }
                }
                break;
            case 'L':
                nflag++;                            /* get name from passwd */
                break;
            case 'U':
                uflag++;                            /* get uid from passwd */
                break;
            case 'G':
                gflag++;                            /* get gid from passwd */
                break;
            case 'N':
                aflag++;                            /* get full name, gecos */
                break;
            case 'H':
                hflag++;                            /* get home folder */
                break;
            case 'S':
                sflag++;                            /* get default shell */
                break;
            case ':':                               /* error - missing operand */
                fprintf(stderr, "Option -%c requires an operand\n", optopt);
                break;
            case '?':                               /* error - unknown option */
                fprintf(stderr,"Unrecognized option: -%c\n", optopt);
        }
    }
    
    bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1)                              /* Value was indeterminate */
        bufsize = 16384;                            /* Should be more than enough */
    
    buf = malloc(bufsize);
    if (buf == NULL) {
        fprintf(stderr,"Buffer malloc error\n");
        exit(EXIT_FAILURE);
    }
    
    /* Print out results for all logins */
    if(lcount > 0){
    for(int i=0; i<lcount; i++){
        s = getpwnam_r(login[i], &pwd, buf, bufsize, &result);
        if (result == NULL) {
            if (s == 0){
                fprintf(stderr,"Chyba: neznamy login %s\n", login[i]);
            }
            else {
                fprintf(stderr,"Chyba getpwnam_r\n");
                exit(EXIT_FAILURE);
            }
        }
        else printf("Name: %s; UID: %ld\n", pwd.pw_gecos, (long) pwd.pw_uid);
    }
    }
    
    /* Print out results for all UIDs */
    if(ucount > 0){
        for(int i=0; i<ucount; i++){
            
        }
        
    }
    
    
    free(buf);
    exit(EXIT_SUCCESS);
}