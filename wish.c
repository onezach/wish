// Author: Zach Potter

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

void _path(int *pathsCount, char *paths[100], int *args_count, char *args[10]) {
    *pathsCount = 0; 
    for (int i = 0; i < *args_count; i++) {
        paths[i] = args[i];
        *pathsCount += 1;
    }
}

void wish(char *paths[100], int *pc, char *in) {

    int *pathsCount = pc;

    if (strcmp(in,"\n") == 0) { return; } 
    char *pre = strsep(&in,"\n");
    char br[100]; 
    char br2[100];
    strcpy(br,pre); 
    
    int size = (int) strlen(pre);
    char *br1 = br;
    while (br1[0] == ' ') { br1++; size--; } 
    if (size == 0) { return; }
    while (br1[size-1] == ' ') { size--; }
    br1[size] = '\0';

    int diff = 0;
    int endred = 0;
    for (int i = 0; i < size; i++) {
        if (br1[i+1] == '>' && br1[i] != ' ') {
            br2[i+diff] = br1[i];
            br2[i+1+diff] = ' ';
            diff++;
        }

        else if (br1[i] == '>' && br1[i+1] != ' ') {
            if (i+1 == size) {
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
                endred = 1;
                break;
            }
            br2[i+diff] = br1[i];
            br2[i+1+diff] = ' ';
            diff++;
        }

        else {
            br2[i+diff] = br1[i];
        }
    }
    if (endred) { return; }
    br2[size+diff] = '\0';
    char *tot = strdup(br2);
    char *h = strdup(br2);

    char *command = strsep(&tot," ");

    FILE *hist; hist = fopen("./.hist", "a");
    if (hist == NULL) {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        return;
    }
    if (strcmp(command,"hist") != 0) { fprintf(hist, "%s\n", h); }
    fclose(hist);

    int args_count = 0;
    char *args[10];
    while ((args[args_count] = strsep(&tot," "))) { args_count++; }

    if (strcmp(command,"exit") == 0) { 
        if (args_count != 0) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            return;
        }
        exit(0); 
    }

    if (strcmp(command,"chist") == 0) {
        if (args_count != 0) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            return;
        }
        hist = fopen("./.hist", "w");
        if (hist == NULL) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            return;
        }
        fclose(hist);
        return;
    }

    if (strcmp(command,"hist") == 0) {
        if (args_count != 0) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            return;
        }
        hist = fopen("./.hist", "r");
        if (hist == NULL) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            return;
        }
        char c = fgetc(hist);
        while (c != EOF) {
            printf("%c",c);
            c = fgetc(hist);
        }
        fclose(hist);
        return;
    }
    
    int loop = 0;
    int loop_count = 0;
    int problem = 0;
    if (strcmp(command,"loop") == 0) { 
        if (args_count == 0) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            return;
        }
        loop = 1; 
        for (int i = 0; i < (int)strlen(args[0]); i++) {
            if (args[0][i] < 48 || args[0][i] > 57) {
                problem = 1;
            }
        }
        if (problem) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            return;
        }
        loop_count = atoi(args[0]); 
        command = args[1];
        for (int i = 0; i < args_count-2; i++) {
            args[i] = args[i+2];
        }
        args_count -= 2;
    }
    
    if (strcmp(command, "path") == 0) { _path(pathsCount, paths, &args_count, args); return; }

    if (strcmp(command, "cd") == 0) {
        if (args_count != 1 || chdir(args[0]) != 0)  {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        return;
    }            

    char path[100];
    int valid = 0;
    for (int i = 0; i < *pathsCount; i++) {
        strcpy(path,paths[i]);
        strcat(path,"/");
        strcat(path,command);
        if (access(path,X_OK) == 0) { valid++; break; }
    }
    if (!valid) {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        return;
    }
    
    char *cmd_argv[args_count+2];
    cmd_argv[0] = strdup(command);
    int redirect = 0;
    char output[100];

    for (int i = 1; i <= args_count; i++) {
        if (strcmp(args[i-1],">") == 0) {
            redirect = 1;
            if (i < args_count) {
                strcpy(output,args[i]);
            } else {
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(0);
            }
            if (i + 1 != args_count) {
                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(0);
            }
            break;
        }       
        cmd_argv[i-redirect] = strdup(args[i-1]);
    }                                                    
    cmd_argv[args_count+1-(2*redirect)] = NULL;
        
    if (loop) {
        for (int i = 1; i <= loop_count; i++) {
            int rc = fork();    

            if (rc == 0) {

                if (redirect) {
                    (void) close(STDOUT_FILENO);
                    open(output, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                }

                for (int x = 0; x < args_count+1-(2*redirect); x++) {
                    if (strcmp(cmd_argv[x],"$loop") == 0) {
                        sprintf(cmd_argv[x],"%d",i);
                    }
                }
                execv(path, cmd_argv);
                printf("failed\n");
                exit(0);     
            } else if (rc > 0) {
                (void)wait(NULL);
            }
        }
    } else {
        int rc = fork();

        if (rc == 0) {
            if (redirect) {
                (void) close(STDOUT_FILENO);
                open(output, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            }
            execv(path, cmd_argv);
            printf("failed\n");
            exit(0);
        } else if (rc > 0) {
            (void)wait(NULL);
        }
    }
}

int main(int argc, char *argv[]) {

    char *paths[100];
    paths[0] = "/bin";
    paths[1] = "/usr/bin";
    int pathsCount = 2;
    char *in = NULL; 
    size_t bufsize = 0;

    if (argc > 1) { // batch mode
        if (argc != 2) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }  

        FILE *batch; batch = fopen(argv[1], "r");
        if (batch == NULL) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }
        while (getline(&in, &bufsize, batch) != -1) {
            wish(paths, &pathsCount, in);
        }
        fclose(batch);
        exit(0);
    } 

    else { // interactive mode
        while (1) {
            printf("wish> ");
            getline(&in, &bufsize, stdin);
            wish(paths, &pathsCount, in);
        }
    }
    return 0;
}
