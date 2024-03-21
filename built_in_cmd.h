#ifndef BUILT_IN_CMD
#define BUILT_IN_CMD

int sh_cd(char **args);

int sh_exit(char **args);

int sh_mkdir(char **args);

char *built_in_cmd[] = {
    "cd",
    "exit",
    "mkdir"
};

int (*built_in_func[]) (char **) = {
    &sh_cd,
    &sh_exit,
    &sh_mkdir
};

#define BUILT_IN_NUM sizeof(built_in_cmd)/sizeof(char *)

#endif
