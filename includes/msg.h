#ifndef __MSG__
#define __MSG__

typedef struct msg{
    int pid;
    int type; //1 inicio do programa ; 2 fim do programa ; 3 status
    char prog_name[20];
    int time;
} Msg;

#endif