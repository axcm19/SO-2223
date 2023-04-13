#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../includes/prog.h"

typedef struct map{
    int pid;
    int time;
    char
} Map;

struct map arr_map[10000];
int posicao = 0;


int guardaNoArray(Prog prog){
    Map map;
    map.pid = prog.pid;
    map.time = prog.time;
    arr_map[posicao] = map;
    printf("mapa pid=%d  time=%d\n",arr_map[posicao].pid,arr_map[posicao].time);
    posicao++;
    return 1;
}

int removeDoArray(aux){
    int i = 0;
    for(i;i<posicao;i++){
        if(arr_map[i].pid == aux.pid){
            for(int j = i;j<posicao;j++){
                arr_map[j] = arr_map[j+1];
            }
            escreveFicheiro(aux);
            posicao--;
        }
    }
}

void escreveFicheiro(aux){
    //
}

int printStatus(){
    //
}

int main(int argc, char **argv) {

    // SERVIDOR LÊ DO PIPE 
    while(1){

        int fd = open("FIFO", O_RDONLY);

        if(fd < 0){
            perror("Erro no open!\n");
        }
        
        int res;
        //char buf[50];
        //while((res = read(fd, &buf, 50)) > 0){
        //char* buf;
        //Prog* p = (Prog*) malloc(sizeof(Prog));
        //Prog* p2 = (Prog*) malloc(sizeof(struct prog));
        Prog aux;
        
        while((res = read(fd, &aux, sizeof(Prog))) > 0){
            write(1, &aux, res); // escreve no terminal
            printf("\nLi do pipe %d \n", aux.pid);
            printf("1\n");
            guardaNoArray(aux);
            write(1,)
            printf("2\n");
            printf("\nLi do pipe %d \n", aux.time);

            /*
            if(aux.type == 1){
                guardaNoArray(aux);

            }
            else if(aux.type == 2){
                removeDoArray(aux);
            }
            else{
                printStatus();
            }
            */

        }


            
        close(fd);

    }
        
    return 0;

}

