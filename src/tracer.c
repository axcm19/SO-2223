#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "../includes/prog.h"
#include "../includes/msg.h"


//################################################################################################################################


// Função que converte uma string em uma estrutura Prog
Prog* parse_single(char* str) {

    char* straux = strdup(str);
    // Aloca memória para a estrutura Prog
    Prog* p = (Prog*) malloc(sizeof(struct prog));
    if (p == NULL) {
        printf("Erro: memória insuficiente!\n");
        return NULL;
    }

    // Define o nome do programa como a primeira palavra da string
    char* nome = strtok(straux, " ");
    if (nome == NULL) {
        printf("Erro: string vazia!\n");
        free(p);
        return NULL;
    }
    p->prog_name = (char*) malloc(strlen(nome) + 1);
    strcpy(p->prog_name, nome);

    // Conta o número de argumentos
    int num_args = 0;
    char* arg;
    while ((arg = strtok(NULL, " "))) {
        num_args++;
    }
    p->num_args = num_args + 1;

    // Aloca memória para os argumentos
    p->arguments = (char**) malloc((num_args + 1) * sizeof(char*));
    if (p->arguments == NULL) {
        printf("Erro: memória insuficiente!\n");
        free(p->prog_name);
        free(p);
        return NULL;
    }

    // Preenche os argumentos
    int i = 1;
    straux = strdup(str);// duplica a string original
    strtok(straux, " "); // ignora a primeira palavra (o nome do programa)

    p->arguments[0] = p->prog_name;
    
    while(arg = strtok(NULL, " ")){
        p->arguments[i] = (char*) malloc(strlen(arg) + 1);
        strcpy(p->arguments[i], arg);
        i++;
    }
    p->arguments[i] = NULL; // ultimo argumento é NULL

    free(straux); // libera a memória alocada por strdup()

    return p;
}



//################################################################################################################################


char** parse_pipeline(const char* commandString, int* numCommands) {
    // Conta o número de comandos separados por "|"
    int count = 1;
    for (int i = 0; commandString[i] != '\0'; i++) {
        if (commandString[i] == '|')
            count++;
    }

    // Aloca memória para o array de comandos
    char** commands = (char**)malloc(count * sizeof(char*));
    if (commands == NULL) {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }

    // Copia cada comando para o array de comandos
    char* commandCopy = strdup(commandString);
    char* token = strtok(commandCopy, "|");
    int i = 0;
    while (token != NULL) {
        commands[i] = strdup(token);
        token = strtok(NULL, "|");
        i++;
    }
    free(commandCopy);

    // Atualiza o número de comandos
    *numCommands = count;

    return commands;
}


int exec_command(char* cmd){
    char* args[10]; // assumimos que nenhum comando tem mais que 10 argumentos
    int i = 0;

    char* command = strdup(cmd);
    char *string = strtok(command, " ");

    while(string != NULL){
        args[i] = string;
		i++;
		string = strtok(NULL, " ");
	}
    args[i] = NULL;
    return execvp(args[0], args);
}


void execute_pipeline(char** commands, int num){
    
    int n_comandos = num;
    int pipes[n_comandos - 1][2]; 

    for(int i = 0; i < n_comandos; i++){
        if(i == 0){
            //cabeça da pipeline
            pipe(pipes[i]);

            int fres = fork();
            if(fres == 0){
                //codigo do filho

                close(pipes[i][0]); //fechar canal de leitura 
                dup2(pipes[i][1], 1);
                close(pipes[i][1]);

                exec_command(commands[i]);
            }
            else{
                //codigo do pai
                close(pipes[i][1]); //fechar canal de escrita 
            }
        }
        if(i == n_comandos - 1){
            //cauda do pipeline
            int fres = fork();
            if(fres == 0){
                //codigo do filho
                dup2(pipes[i-1][0], 0);     
                close(pipes[i-1][0]); //fechar canal de leitura 

                exec_command(commands[i]);
            }
            else{
                //codigo do pai
                close(pipes[i-1][0]); //fechar canal de leitura
            }
        }   
        else{
            // nem na cabeça nem na cauda
            pipe(pipes[i]);
            int fres = fork();
            if(fres == 0){
                //codigo do filho
                dup2(pipes[i-1][0], 0);
                close(pipes[i-1][0]);
                dup2(pipes[i][1], 1);
                close(pipes[i][1]);
                close(pipes[i][0]);

                exec_command(commands[i]);
            }
            else{
                //codigo do pai
                close(pipes[i][1]);
                close(pipes[i-1][0]);
            }
        }
    }
    
    int j;
    for(j = 0; j < n_comandos; j++){
        wait(NULL);
    }
}


//################################################################################################################################


int main(int argc, char **argv) {

    int fres = 0;
    int fres2 = 0;

    if(argc < 2){
        printf("Falta de argumentos!\n");
        return -1;
    }

    else{


        char* option = strdup(argv[1]); // pode ser "execute" ou "status"
        //char* flag = strdup(argv[2]);  // pode ser "-u" para programa individual ou "-p" para pipeline de programas
        //char* programs = strdup(argv[3]);  // string que contem os programas a executar mais os argumentos
        

        if(strcmp(option, "execute") == 0){

            char* flag = strdup(argv[2]);  // pode ser "-u" para programa individual ou "-p" para pipeline de programas
            char* programs = strdup(argv[3]);  // string que contem os programas a executar mais os argumentos

            if(strcmp(flag, "-u") == 0){

                int begin_time_in_mill = 0;
                int end_time_in_mill = 0;

                // gera um novo processo
                fres = fork();

                if(fres == 0){
                    // codigo do processo filho

                    //-----------------------------------------
                    // Obter tempo inicial

                    struct timeval begin, end;
                    gettimeofday(&begin, NULL);
                    begin_time_in_mill = (begin.tv_sec) * 1000 + (begin.tv_usec) / 1000;

                    //-----------------------------------------
                    
                    // fazer parse single
                    Prog* p = parse_single(programs);
                    int new_pid = getpid();
                    p->pid = new_pid;
                    p->time = begin_time_in_mill;

                    printf("\n");
                    printf("Running PID %d\n", p->pid);
                    printf("\n");

                    //-----------------------------------------
                    
                    int fd = open("FIFO", O_WRONLY);
                    if(fd < 0){
                        perror("Erro no open!\n");
                    }

                    //------------------------------------------------------------------

                    Msg msg;
                    strcpy(msg.prog_name,p->prog_name);
                    msg.pid=p->pid;                     //nova estrurura para enviar
                    msg.time = p->time;
                    msg.type = 1;

                    //------------------------------------------------------------------
                    
                    printf("\n");
                    write(fd, &msg, sizeof(Msg));

                    // execução do programa
                    if(msg.type == 1){

                        fres2 = fork();

                        if(fres2 == 0){
                            int res = execvp(p->prog_name, p->arguments);
                            _exit(-1);
                        }
                        int status;
                        wait(&status);
                        if(WEXITSTATUS(status) < 255){

                            gettimeofday(&end, NULL);
                            //get the total number of ms that the code took:
                            end_time_in_mill = ((end.tv_sec) * 1000 + (end.tv_usec) / 1000) - begin_time_in_mill;

                            printf("\n");
                            printf("Ended in %d ms\n", end_time_in_mill);
                            printf("\n");
                        }
                        else{
                            printf("\n");
                            printf("ERROR!\n");
                            printf("\n");
                        }
                    }

                    //-----------------------------------------

                    msg.type = 2;
                    msg.time = end_time_in_mill;

                    write(fd,&msg,sizeof(Msg));
                    free(p);
                    close(fd);

                    _exit(1);  // caso haja problemas no execvp
                }


                
                // codigo do processo pai
                int status;
                wait(&status);
                if(WEXITSTATUS(status) < 255){
                    printf("\n");
                    printf("\n");
                }
                else{
                    printf("\n");
                    printf("ERROR!\n");
                    printf("\n");
                }
            }
            else if(strcmp(flag, "-p") == 0){
                // fazer parse do pipe

                int begin_time_in_mill = 0;
                int end_time_in_mill = 0;

                int fres3 = fork();
                if(fres3 == 0){

                    //------------------------------------------------------------------
                
                    int pid = getpid();

                    struct timeval begin, end;
                    gettimeofday(&begin, NULL);
                    begin_time_in_mill = (begin.tv_sec) * 1000 + (begin.tv_usec) / 1000;

                    printf("\n");
                    printf("Running PID %d\n", pid);
                    printf("\n");

                    int fd = open("FIFO", O_WRONLY);
                    if(fd < 0){
                        perror("Erro no open!\n");
                    }


                    //------------------------------------------------------------------

                    Msg msg;
                    strcpy(msg.prog_name, programs);
                    msg.pid = pid;                     //nova estrurura para enviar
                    msg.time = begin_time_in_mill;
                    msg.type = 1;

                    printf("\n");
                    write(fd, &msg, sizeof(Msg));

                    //------------------------------------------------------------------

                    int num;
                    char** commands = parse_pipeline(programs, &num);
                    execute_pipeline(commands, num);  

                    gettimeofday(&end, NULL);
                    //get the total number of ms that the code took:
                    end_time_in_mill = ((end.tv_sec) * 1000 + (end.tv_usec) / 1000) - begin_time_in_mill;

                    printf("\n");
                    printf("Ended in %d ms\n", end_time_in_mill);
                    printf("\n");

                    //------------------------------------------------------------------

                    msg.type = 2;
                    msg.time = end_time_in_mill;

                    write(fd,&msg,sizeof(Msg));
                    close(fd);

                    _exit(1);  
                }
                // codigo do processo pai
                int status;
                wait(&status);
                if(WEXITSTATUS(status) < 255){
                    printf("\n");
                    printf("\n");
                }
                else{
                    printf("\n");
                    printf("ERROR!\n");
                    printf("\n");
                }
            }
        }

        else if(strcmp(option, "status") == 0){

                int id = getpid();
                
                int fd = open("FIFO", O_WRONLY);
                if(fd < 0){
                    perror("Erro no open!\n");
                }

                char fifoname[100];
                sprintf(fifoname,"FIFO-%d", id);

                int fazFifo = mkfifo(fifoname, 0640);
                if(fazFifo < 0){
                    perror("Erro no mkfifo Status!\n");
                }

                //------------------------------------------------------------------
                
                Msg msg;
                strcpy(msg.prog_name, fifoname);
                msg.pid = id;
                msg.type = 3; 

                //------------------------------------------------------------------
            
                write(fd, &msg, sizeof(Msg));
                close(fd);
                
                char message[100];
                int res;

                int fdstatus = open(fifoname, O_RDONLY);
                if(fdstatus < 0){
                    perror("Erro no open do Status!\n");
                }

            
                while((res = read(fdstatus, &message, strlen(message))) > 0){
                    write(1,&message,strlen(message));
                }

                //-----------------------------------------
                 
                close(fdstatus);
                unlink(fifoname);
                printf("Consegui imprimir o status!\n");
            
        }
    }
        
    return 0;

}


//################################################################################################################################