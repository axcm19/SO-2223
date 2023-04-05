#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/time.h>


//################################################################################################################################


// Definição da estrutura Prog
typedef struct prog{
    int pid;
    char* prog_name;
    char** arguments;
    int num_args;
    int time;
} Prog;


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


int parse_pipeline(){
    return 0;
}


//################################################################################################################################


int main(int argc, char **argv) {

    int fres = 0;


    if(argc < 4){
        printf("Falta de argumentos!\n");
        return -1;
    }

    else{


        char* option = strdup(argv[1]); // pode ser "execute" ou "status"
        char* flag = strdup(argv[2]);  // pode ser "-u" para programa individual ou "-p" para pipeline de programas
        char* programs = strdup(argv[3]);  // string que contem os programas a executar mais os argumentos
        

        if(strcmp(option, "execute") == 0 && strcmp(flag, "-u") == 0){

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
                // Testar se a estrutura foi de facto criada

                printf("\n");
                printf("ID Processo: %d\n", p->pid);
                printf("Nome: %s\n", p->prog_name);
                printf("Tempo em que iniciou: %d\n", p->time);
                printf("Argumentos:\n");
                for (int i = 1; i < p->num_args; i++) {
                    printf("  %s\n", p->arguments[i]);
                }
                //-----------------------------------------
                
                int fd = open("FIFO", O_WRONLY);
                if(fd < 0){
                    perror("Erro no open!\n");
                }
                
                printf("\n");
                write(fd, &(p->prog_name), strlen(p->prog_name));

                // execução do programa
                sleep(5);
                int res = execvp(p->prog_name, p->arguments);


                //-----------------------------------------
                // Obter tempo final

                gettimeofday(&end, NULL);
                //get the total number of ms that the code took:
                end_time_in_mill = ((end.tv_sec) * 1000 + (end.tv_usec) / 1000) - begin_time_in_mill;

                //-----------------------------------------

                free(p);
                close(fd);

                sleep(5);
                _exit(-1);  // caso haja problemas no execvp
            }

            
            // codigo do processo pai
            int status;
            wait(&status);
            if(WEXITSTATUS(status)){
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

        if(strcmp(option, "execute") == 0 && strcmp(flag, "-p") == 0){
            // fazer parse do pipe
        }

        if(strcmp(option, "status") == 0){
            
        }


    }
        
    return 0;

}


//################################################################################################################################