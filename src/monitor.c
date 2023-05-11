#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "../includes/prog.h"
#include "../includes/msg.h"


typedef struct map{
    int pid;
    int time;
    char nome[20];
} Map;

struct map arr_map[1000000];
int posicao = 0;


int guardaNoArray(Msg prog){
    Map map;
    map.pid = prog.pid;
    map.time = prog.time;
    strcpy(map.nome,prog.prog_name);
    arr_map[posicao] = map;
    posicao++;
    return 1;
}


void escreveFicheiro(Msg aux){
    char nomeFicheiro[50];
    char conteudoFicheiro[100];
    int time = 0;
    struct timeval end;
    gettimeofday(&end, NULL);
    time = (end.tv_sec) * 1000 + (end.tv_usec) / 1000;
    sprintf(nomeFicheiro,"../PIDS-folder/%d.txt",aux.pid);
    sprintf(conteudoFicheiro,"Nome: %s\nTempo de Execucao: %dms\n",aux.prog_name,time);
    int file = open(nomeFicheiro, O_RDWR | O_CREAT,0666);
    write(file,conteudoFicheiro,strlen(conteudoFicheiro));
}


void removeDoArray(Msg aux){
    int i = 0;
    for(i = 0; i < posicao; i++){
        if(arr_map[i].pid == aux.pid){
            for(int j = i;j<posicao;j++){
                arr_map[j] = arr_map[j+1];
            }
            escreveFicheiro(aux);
            posicao--;
        printf("Removido do array\n\n");
        }
    }
}


int printStatus(int fd){
    int time = 0,final = 0;
    struct timeval begin;
    char message[100];
    for(int i = 0;i<posicao;i++){
        gettimeofday(&begin, NULL);
        time = (begin.tv_sec) * 1000 + (begin.tv_usec) / 1000;
        final = time - arr_map[i].time;
        sprintf(message,"%d %s %d ms\n", arr_map[i].pid, arr_map[i].nome, final);
        write(fd,&message,strlen(message));
        write(1,&message,strlen(message));    
    }
    return 1;
}

int main(int argc, char **argv) {

    // criação da diretoria PIDS-folder onde são guardados os ficheiros de programas terminados 
    int result = mkdir("../PIDS-folder", 0777);

    // SERVIDOR LÊ DO PIPE 
    while(1){

        int fd = open("FIFO", O_RDONLY);

        if(fd < 0){
            perror("Erro no open!\n");
        }

        int res;
        Msg aux;

        
        while((res = read(fd, &aux, sizeof(Msg))) > 0){

            if(aux.type ==1){
                printf("\nLi do pipe %d \n", aux.pid);
                guardaNoArray(aux);
            }
            else if(aux.type == 2){
                removeDoArray(aux);
            }
            else if(aux.type == 3){

                int fres = fork();
                if(fres == 0){

                    int time = 0,final = 0;
                    struct timeval begin;
                    char message[100];

                    int fdstatus = open(aux.prog_name, O_WRONLY);

                    if(fdstatus < 0){
                        perror("Erro no open do Status!\n");
                    }
                    
                    
                    for(int i = 0; i < posicao; i++){

                        gettimeofday(&begin, NULL);
                        time = (begin.tv_sec) * 1000 + (begin.tv_usec) / 1000;
                        final = time - arr_map[i].time;
                        sprintf(message,"%d %s %d ms\n",arr_map[i].pid, arr_map[i].nome, final);
                        write(fdstatus,&message,strlen(message));
                        write(1,&message,strlen(message));
                    }

                    close(fdstatus); 
                    _exit(1);
                }
                else{
                    // codigo do processo pai
                    int status;
                    wait(&status);
                    if(WEXITSTATUS(status) < 255){
                        printf("\n");
                        printf("Imprimi o status!");
                        printf("\n");
                    }
                    else{
                        printf("\n");
                        printf("ERROR!\n");
                        printf("\n");
                    }
                }
            }
        }

        close(fd);

    }
        
    return 0;

}

