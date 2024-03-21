#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>


//#ifndef BUILT_IN_CMD
//#define BUILT_IN_CMD
#include "built_in_cmd.h"
#define BUILT_IN_CMD
//#endif

// removeChar(s, index) removes the char in postion index in the string s from s.
// requires: 0 <= index < strlen(s)
// effects: mutates heap variable.
void removeChar(char *s, int index){
    assert (index > 0);
    assert (index >= strlen(s));

    strcpy(&s[index], &s[index + 1]);
}

// check_string_memory(pos, len, s) checks whether the string has used out of memory
// effects: allocates heap memory [caller must free]
char *check_string_memory(int pos, int* len, char *s){
    if (pos == *len){
            (*len) *= 2;
            char *new_s = realloc(s, sizeof(char) * (*len));
            if (new_s == NULL){
                // out of memory
                fprintf(stderr,"check_string_memory:Out of memory!!!\n");
                free(s);
                return NULL;
            }
            else {
                return new_s;
            }
        }
    else return s;
}


// sh_read_line() reads a line from the console and returns the remainder of the line from the input
// effects: allocates heap memory [caller must free]
char *sh_read_line(){
    int line_len = 1;
    int pos = 0;
    char *line = malloc(sizeof(char) * line_len);

    char new_c;
    while (true){

        new_c = getchar();
        //printf("%c ",new_c);

        if (new_c == EOF || new_c == '\n'){
            line[pos]='\0';
            break;
        }

        line[pos] = new_c;
        pos++;
        /*
        char *new_line = check_string_memory(pos, &line_len, line);
        if (new_line == NULL){
            fprintf(stderr,"Error: line[] is out of memory.");                        
            //free(line);
            exit(EXIT_FAILURE);
        }
        else{
            line = new_line;
        }*/
        
        if (pos == line_len){
            line_len *= 2;
            line = realloc(line, sizeof(char) * line_len);
            if (line == NULL){
                // out of memory
                fprintf(stderr,"Error: Out of memory!!!\n");
                free(line);
                return NULL;
            }
        }
    }
    
    if (pos != 0 && line != NULL){
        return line;
    }
    else {
        free(line);
        return NULL;
    }
}

// sh_parse(line) parses the line
// effects: allocates heap memory [caller must free]
// requires: line should not be NULL
char **sh_parse(char *line){
    //assert (line);

    int line_index = 0;

    int args_pos = 0;
    int args_len = 1;
    char **args = malloc(args_len * sizeof(char*));
    
    if (args == NULL){
        // out of memory
        fprintf(stderr,"Error: args[] is out of memory.\n");
        free(line);
        exit(EXIT_FAILURE);
    }

    
    bool double_quote = false;
    bool new_token = true;
    bool pre_space = true;
    
    while (line[line_index] != '\0'){

        if (line[line_index] == '"'){
            if(double_quote){
                line[line_index] = '\0';
            }
            if (!pre_space){
                removeChar(line, line_index);
            }
            double_quote = !double_quote;
            line_index++;
            pre_space = false;
            continue;
        }
        
        if (line[line_index] == ' ' && !double_quote){   // space
            new_token = true;
            line[line_index]='\0';
            line_index++;
            pre_space = true;
                
        }
        else if (new_token == true){ // character and new token
            args[args_pos] = line + line_index;
            args_pos++;
            line_index++;
            new_token = false;
            pre_space = false;
            if (args_pos == args_len){
                args_len *= 2;
                char **new_args = realloc(args, args_len * sizeof(char*));
                if (new_args == NULL){
                    // out of memory
                    fprintf(stderr,"Error: args[] is out of memory.\n");
                    free(line);
                    free(args);
                    exit(EXIT_FAILURE);
                }
                else{
                    args = new_args;
                }
            }
        }
        else {  //character but not new token
            line_index++;                
        }
        
    }
    args[args_pos] = NULL;
    return args;

}

// sh_launch(args) launches the program with arguments args
int sh_launch(char **args){
    int status;
    pid_t pid = fork();
    pid_t wait_pid;

    if (pid == 0){ //child process
        if (execvp(args[0], args) == -1){
            perror("sh_launch: execvp failed:");
                        
            free(args[0]);
            free(args);

            exit(EXIT_FAILURE);
        }
    }
    else if (pid < 0){ //error
        perror("sh_launch: fork failed:");
                  
        free(args[0]);
        free(args);

        exit(EXIT_FAILURE);

    }
    else{   //parent process
        do{            
            waitpid(pid, &status, WUNTRACED);            
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;

}


// sh_execute(args) executes the command with arguments args
// requires: args[0] != NULL;
int sh_execute(char **args){
    assert (args[0] != NULL);

    for (int i = 0; i < BUILT_IN_NUM; i++){
        if (strcmp (args[0], built_in_cmd[i]) == 0){
            return built_in_func[i](args);
        }
    }

    return sh_launch(args);
}


void print_args(char **args){
    
    for (char **p = args; *p != NULL; p++){        
        printf("%s\n",*p);
    }
    
}



// sh_loop() 
void sh_loop(void){
    char *line = NULL;
    char **args = NULL;
    int status = 1;
    char cwd[PATH_MAX];

    do{
        // print current root
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s", cwd);
        } else {
            perror("getcwd");
            return;
        }
        printf("$ ");

        
        line = sh_read_line();
        //printf("%s",line);
        //if (line == NULL)break;
        args = sh_parse(line);
        //free(args[0]);

        //print_args(args);
        
        status = sh_execute(args);
        free(line);
        free(args);

    } while(status);
    //free(cwd);

}


int main(int argc, char *argv[]){
    sh_loop();
    return 0;
}