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
"* Author:  Salamander\n"
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
    struct passwd pwd, *result;
    char *buf, *optstring = "l:u:LUGNHS", *login[MAX], *next;
    int s, c, index;
    unsigned int uid[MAX];
    size_t nflag=0, uflag=0, gflag=0, aflag=0, hflag=0, sflag=0, lcount=0, ucount=0, bufsize, written=0;
    extern int optopt, optind;
    
    if (argc < 3){
		fprintf(stdout,"%s\n",HELPFILE);
		return EXIT_FAILURE;
	}
    
    while((c = getopt(argc, argv, optstring)) != -1){
        switch(c){
            case 'l':
                index = optind-1;
                while(index < argc){
                    next = strdup(argv[index]);    /* get logins */
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
                    next = strdup(argv[index]);    /* get uids */
                    index++;
                    if(next[0] != '-'){             /* check if optarg isn't next switch */
                        uid[ucount++] = atoi(next);
                    }
                    else break;
                }
                optind = index-1;                   /* set optind to correct value */
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
                break;
        }
    }
    
    if((lcount == 0) && (ucount == 0)){
        fprintf(stderr, "No UID or users entered.\n");   /* nothing to do -> exit */
        return EXIT_FAILURE;
    }
    
    bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if ((int)bufsize == -1)                              /* Value was indeterminate */
        bufsize = 16384;                                /* Should be more than enough */
    
    buf = malloc(bufsize);
    if (buf == NULL) {
        fprintf(stderr,"Buffer malloc error\n");
        return EXIT_FAILURE;
    }
    
    /* Print out results for all logins */
    if(lcount > 0){
        for(unsigned int i=0; i<lcount; i++){
            s = getpwnam_r(login[i], &pwd, buf, bufsize, &result);              /* get user data from /etc/passwd */
            if (result == NULL) {
                if (s == 0){
                    fprintf(stderr,"Chyba: neznamy login %s\n", login[i]);      /* set correct error code ! */
                }
                else {
                    fprintf(stderr,"Chyba getpwnam_r\n");                       /* set correct error code ! */
                    return EXIT_FAILURE;
                }
            }
            else {
                if(nflag){fprintf(stdout,"%s",pwd.pw_name); written++;}
                if(uflag){
                    if(written){fprintf(stdout,", "); written=0;}
                    fprintf(stdout,"%zd",(ssize_t)pwd.pw_uid); written++;
                }
                if(gflag){
                    if(written){fprintf(stdout,", "); written=0;}
                    fprintf(stdout,"%zd",(ssize_t)pwd.pw_gid); written++;
                }
                if(aflag){
                    if(written){fprintf(stdout,", "); written=0;}
                    fprintf(stdout,"%s",pwd.pw_gecos); written++;
                }
                if(hflag){
                    if(written){fprintf(stdout,", "); written=0;}
                    fprintf(stdout,"%s",pwd.pw_dir); written++;
                }
                if(sflag){
                    if(written){fprintf(stdout,", "); written=0;}
                    fprintf(stdout,"%s",pwd.pw_shell);}
                fprintf(stdout,"\n");
            }
        }
    }
    
    /* Print out results for all UIDs */
    if(ucount > 0){
        for(unsigned int i=0; i<ucount; i++){
            s = getpwuid_r(uid[i], &pwd, buf, bufsize, &result);                    /* get user data from /etc/passwd */
            if(result == NULL){
                if (s == 0){
                    fprintf(stdout,"Chyba: nezname UID %zd\n", (ssize_t)uid[i]);    /* set correct error code ! */
                }
                else {
                    fprintf(stderr,"Chyba getpwuid_r\n");                           /* set correct error code ! */
                    return EXIT_FAILURE;
                }
            }
            else {
                if(nflag){fprintf(stdout,"%s",pwd.pw_name); written++;}
                if(uflag){
                    if(written){fprintf(stdout,", "); written=0;}
                    fprintf(stdout,"%zd",(ssize_t)pwd.pw_uid); written++;
                }
                if(gflag){
                    if(written){fprintf(stdout,", "); written=0;}
                    fprintf(stdout,"%zd",(ssize_t)pwd.pw_gid); written++;
                }
                if(aflag){
                    if(written){fprintf(stdout,", "); written=0;}
                    fprintf(stdout,"%s",pwd.pw_gecos); written++;
                }
                if(hflag){
                    if(written){fprintf(stdout,", "); written=0;}
                    fprintf(stdout,"%s",pwd.pw_dir); written++;
                }
                if(sflag){
                    if(written){fprintf(stdout,", "); written=0;}
                    fprintf(stdout,"%s",pwd.pw_shell);}
                fprintf(stdout,"\n");
            }
        }
    }
  
    free(buf);
    return EXIT_SUCCESS;
}