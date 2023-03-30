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

    // SERVIDOR LÊ DO PIPE 
    while(1){

        int fd = open("FIFO", O_RDONLY);

        if(fd < 0){
            perror("Erro no open!\n");
        }
        
        int res;
        char buf[50];
        //while((res = read(fd, &buf, 50)) > 0){

        while((res = read(fd, &buf, 50)) > 0){
            write(1, &buf, res); // escreve no terminal
            printf("\nLi do pipe %s \n", buf);
        }
            
            
        close(fd);

    }
        
    return 0;

}

