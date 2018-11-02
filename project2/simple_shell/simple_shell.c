#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h> 
#include <stdlib.h>
#include <string.h> 
#include <fcntl.h>

#define MAX_LINE 80 /* The maximum len command */
#define OPTIONS_NUM 5 /* the shell parameter number */

int shell_parser(char * command, char ** args, char ** option)
/* parse command to a array, return end if succeed 0 if else */
{
    int len = strlen(command), end = 0, i = 0, j = 0;
    if(len == MAX_LINE) return -1; // command is too long

    char *buffer = (char*)malloc(MAX_LINE * sizeof (char));
    for(int k = 0; k < OPTIONS_NUM; k++) option[k][0] = 0; /* initialization */

    while(len > 0 && command[len - 1] == ' '){ len--; }/* remove extra blank */

    if(len > 0 && command[len - 1] == '&') {option[0][0] = 1; len--;} /* detect & */
    //printf("detected len = %d\n", len);
    
    while(i < len){
        j = 0; /* buffer length */
        while(i < len && command[i] == ' ') i++;
        while(i < len && command[i] != ' '){
            buffer[j++] = command[i++];
        } 
        buffer[j] = '\0';

        if(j == 0) continue;

        if(j == 1){ /* if single character */
            int op = 0;
            switch (buffer[0]){
                case '>' : op = 2; break;
                case '<' : op = 3; break;
                case '|' : op = 4; break;
            }
            if(op > 0){
                strncpy(option[op], command + i, len - i);
                option[op][len - i] = '\0';
                i = len;
            }
            else{ /* not one of them */
                if(args[end] == NULL) args[end] = (char*) malloc(MAX_LINE * sizeof(char));
                strcpy(args[end], buffer);
                //printf("detected single args: %s\n", args[end]);
                end++;
            }
        }
        
        /* if the command is !! */
        else if (j == 2 && buffer[0] == '!' && buffer[1] == '!')
            option[1][0] = 1;
        
        else {
            if(args[end] == NULL) args[end] = (char*) malloc(MAX_LINE * sizeof (char));
            strcpy(args[end], buffer);
            //printf("detected args: %s\n", args[end]);
            end++;
        }
            
        
    }
    /* make other args NULL */
    for(int k = end; k < MAX_LINE/2 + 1; k++)
    {
        if(args[k] != NULL){
            free(args[k]);
            args[k] = NULL;
        }
    }

    free(buffer);
    return end;
}

char * strip(char * str){
    int left = 0, right = strlen(str) - 1, len;
    while(left <= right && str[left] == ' ') left++;
    while(left <= right && str[right] == ' ') right--;
    if(left > right) return NULL;
    len = right - left + 1;
    char * newstr = (char * )malloc((len + 1) * sizeof(char));
    strncpy(newstr, str + left, len);
    newstr[len] = '\0';
    return newstr;
}
int main(void)
{
    char *args[MAX_LINE/2 + 1], *option[OPTIONS_NUM]; /* command line arguments */
    /* 
       option[0]: &?
       option[1]: !!?
       option[2]: redirect > file name, 0 if not exist
       option[3]: redirect < file name, 0 if not exist 
       option[4]: pipe command name, 0 if not exist 
    */


    int should_run = 1; /* flag to determine when to exit progream */
    char history_buff[MAX_LINE], command[MAX_LINE];
    history_buff[0] = '\0';
    int end, i;
    
     /* iniitalization */
    for(i = 0; i < MAX_LINE/2 + 1; i++) args[i] = NULL;
    for(i = 0; i < OPTIONS_NUM; i++) option[i] = (char*)malloc(MAX_LINE);

    while (should_run)
    {
        fflush(stdout);
        //printf("stdout flushed\n");
        printf("simple_shell> ");
        
        /**
         * (1) fork a child process usnig fork()
         * (2) the child process will invoke execvp()
         * (3) parent will invoke wait() unless command included &
         **/
        
        i = 0;
        while(i < MAX_LINE - 1 && (command[i] = getchar()) != '\n') i++;
        command[i] = '\0';
        fflush(stdin);

        // printf("parsing...\n");
        end = shell_parser(command, args, option);

        if(end == -1) {
            printf(">Illegal Command: you may have input too long!\n");
            continue;
        }
        else if(end == 0 && option[1][0] == 0){  
            // printf("blank command detected\n");
            continue;  }

        if(option[1][0] != 0) { 
            strcpy(command, history_buff); /* substitute !! to history command */
            end = shell_parser(command, args, option); /* parse again */
        }

/*         printf("parsing complete:\n%d args detected\n", end);
        printf("args: ");
        for(i = 0; i < end; i++){
            printf("%s ", args[i]);
        }
        printf("\n");

        printf("&: %d\n!!: %d\n", option[0][0], option[1][0]);
        if(option[2][0] != 0) printf("option[2] = %s\n", option[2]);
        else if (option[3][0] != 0) printf("option[3] = %s\n", option[3]);
        else if(option[4][0] != 0) printf("option[4] = %s\n", option[4]);
        else printf("nothing special\n"); */
       

        pid_t pid;
        pid = fork();
        if (pid < 0) {
            /* error occurred */
            fprintf(stderr, "Fork Failed");
            return 1;
        }
        else if (pid == 0){
            /* child process */
            int sfd, /* saved STDOUT_FILENO.or STDIN_FILENO*/
            fd; /* file descriptor */
            char * filename;
            int op, STD_FILENO;
            if(option[2][0] != 0 || option[3][0] != 0){
                op = (option[2][0] != 0)? 2 : 3; 
                STD_FILENO = (option[2][0] != 0)? STDOUT_FILENO : STDIN_FILENO;

                sfd = dup(STD_FILENO);
                filename = strip(option[op]);
                fd = open(filename, O_RDWR | O_CREAT);
                if(fd < 0){
                    printf("open file error\n");
                    exit(1);
                }
                if (dup2(fd, STD_FILENO) < 0) { 
                    printf("can't redirect fd error\n"); 
                    exit(1); 
                }
                execvp(args[0], args);
                /* recover STDOUT_FILENO */
                if (dup2(sfd, STD_FILENO) < 0) { 
                    printf("recover fd failed \n"); 
                }
                if(filename) free(filename);
                close(fd);
            }

            else if(option[4][0] != 0){
                printf("communication via a pipe(to be implemented)\n");
                exit(0);
            }
            else {
                execvp(args[0], args);
            }
            
        }
        else{
            /* parent process */
            /* parent will wait for the child to complete */
            if(option[0][0] == 0)
            {
                printf("no & detected\n");
                /* if no & detected then parent process will wait child process to exit */
                waitpid(0, NULL, 0);
                printf ("Child process quit.\n");
            }
                
            
        }

        strcpy(history_buff, command); /* update history */
    }
    //for(i = 0; i < OPTIONS_NUM; i++) free(option[i]);
    return 0;
}
