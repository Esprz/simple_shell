#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>


// check_string_memory(pos, len, s) checks whether the string has used out of memory
char *check_string_memory(int pos, int* len, char *s){
    if (pos == *len){
            *len *= 2;
            s = realloc(s, sizeof(char) * *len);
            if (s == NULL){
                // out of memory
                printf("Error: Out of memory!!!\n");
                free(s);
                return NULL;
            }
            else {
                return s;
            }
        }
}


// sh_read_line() reads a line from the console and returns the remainder of the line from the input
// effects: allocates heap memory [caller must free]
char *sh_read_line(){
    int line_len = 1;
    int pos = 0;
    char *line = malloc(sizeof(char) * line_len);

    char new_c;
    while (scanf("%d", &new_c) == 1){
        if (new_c == EOF || new_c == '\n'){
            line[pos]='\0';
            break;
        }
        line[pos] = new_c;
        pos++;
        line = check_string_memory(pos, line_len,line);
        if (line == NULL){
            fprintf(stderr,"Error: line[] is out of memory.");                        
            free(line);
            exit(EXIT_FAILURE);
        }
        /*
        if (pos == line_len){
            line_len *= 2;
            line = realloc(line, sizeof(char) * line_len);
            if (line == NULL){
                // out of memory
                printf("Error: Out of memory!!!\n");
                free(line);
                return NULL;
            }
        }*/
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
char *sh_parse(char *line){
    assert (line);

    int line_index = 0;

    int token_pos = 0;
    int token_len = 1;
    char *token = malloc(token_len * sizeof(char)); 

    int args_len = 1;
    int args_pos = 0;
    char **args = malloc(args_len * sizeof(char *));
    if (args == NULL){
        // out of memory
        fprintf(stderr,"Error: args[] is out of memory.");
        free(token);
        free(line);
        free(args);
        //return NULL;
        exit(EXIT_FAILURE);
    }
    args[0] = token;
    args_pos++;
    
    
    bool double_quote = false;
    
    while (line[line_index] != '\0'){
        if (line[line_index] == '"'){
            double_quote = !double_quote;
            line_index++;
            continue;
        }

        // in quote
        if (double_quote){
            token[token_pos] = line[line_index];
            token_pos++;
            line_index++;
            token = check_string_memory(token_pos, &token_len, &token);
            if (token == NULL){
                fprintf(stderr,"Error: token[] is out of memory.");
                free(token);
                free(line);
                free(args);
                exit(EXIT_FAILURE);
            }
        }
        //not in quote
        else{
            if (line[line_index] == ' '){
                token[token_pos]='\0';
                token_pos++;
                line_index++;                    
                token = check_string_memory (token_pos, &token_len, &token);
                if (token == NULL){
                    fprintf(stderr,"Error: token[] is out of memory.");
                    free(token);
                    free(line);
                    free(args);
                    exit(EXIT_FAILURE);
                }
                
                args[args_pos]=token[token_pos];
                args_pos++;
                if (args_pos == args_len){
                    args_len *= 2;
                    args = realloc(args, args_len * sizeof(char*));
                    if (args == NULL){
                        // out of memory
                        fprintf(stderr,"Error: args[] is out of memory.");
                        free(token);
                        free(line);
                        free(args);
                        //return NULL;
                        exit(EXIT_FAILURE);
                    }
                }
                
                    
            }
            else{
                token[token_pos]=line[line_index];
                token_pos++;
                line_index++;
                token = check_string_memory(token_pos, &token_len, &token);
                if (token == NULL){
                    fprintf(stderr,"Error: token[] is out of memory.");
                    free(token);
                    free(line);
                    free(args);
                    exit(EXIT_FAILURE);
                }
            }
        }       
    }
    free(line);
    return args;

}

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


int sh_execute(){


}

// sh_loop() 
void sh_loop(void){
    char *line = NULL;
    char **args = NULL;
    int status;
    do{
        printf("$");
        line = sh_read_line();
        if (line == NULL){//No input
            continue;
        }
        args = sh_parse(&line);
        status = sh_execute();

        
        
    } while(status);

}


int main(int argc, char *argv[]){
    sh_loop();
    return 0;
}