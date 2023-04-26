#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char **argv) {


    int fd = mkfifo("FIFO", 0640);
    if(fd < 0){
        perror("Erro no mkfifo!\n");
    }
    int fdstatus = mkfifo("FIFOSTATUS", 0640);
    if(fdstatus < 0){
        perror("Erro no mkfifo Status!\n");
    }
        
        
    return 0;

}