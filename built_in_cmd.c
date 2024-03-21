#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include <libgen.h>
#define EXEC_SUCCESSED 1
#define EXEC_FAILED -1
#define EXIT 0

//#include "built_in_cmd.h"
#ifndef BUILT_IN_CMD
#define BUILT_IN_CMD
#include "built_in_cmd.h"
#endif

// sh_cd(args) change the working directory to args[1]
int sh_cd(char **args){
    
    if (args[1] == NULL){
        char exe_path[PATH_MAX];
        if (realpath(".", exe_path) == NULL){
            perror("sh_cd: exe_path: realpath");
            return EXEC_FAILED;
        }
        char *exe_dir = dirname(exe_path);
        if (chdir(exe_dir) != 0) {
            perror("sh_cd:");
            return EXEC_FAILED;
        }

    }
    else if (args[2] != NULL){
        fprintf(stderr, "sh_cd: too many arguments\n");
        return EXEC_FAILED;
    }
    else if (chdir(args[1]) != 0){
        perror("sh_cd:");
        return EXEC_FAILED;
    }
    return EXEC_SUCCESSED;    

}

int sh_exit(char **args){
    return EXIT;
}

// sh_mkdir(args) makes a directory with name of args[1] in the working directory
int sh_mkdir(char **args){

    if (args[1] == NULL){
        fprintf(stderr, "sh_mkdir: missing arguments\n");
        return EXEC_FAILED;
    }
    else if (args[2] != NULL){
        fprintf(stderr, "sh_mkdir: too many arguments\n");
        return EXEC_FAILED;
    }
    else if (mkdir(args[1], 0777) != 0) {
        perror("sh_mkdir:");
        return EXEC_FAILED;
    }
    
    return EXEC_SUCCESSED;
}