#ifndef __PROG__
#define __PROG__

typedef struct prog{
    int pid;
    char* prog_name;
    char** arguments;
    int num_args;
    int time;
} Prog;

#endif