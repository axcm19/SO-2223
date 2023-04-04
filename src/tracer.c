#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>


//################################################################################################################################


// Definição da estrutura Prog
typedef struct prog{
    int pid;
    char* prog_name;
    char** arguments;
    int num_args;
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
    p->num_args = num_args;

    // Aloca memória para os argumentos
    p->arguments = (char**) malloc((num_args + 1) * sizeof(char*));
    if (p->arguments == NULL) {
        printf("Erro: memória insuficiente!\n");
        free(p->prog_name);
        free(p);
        return NULL;
    }

    // Preenche os argumentos
    int i = 0;
    straux = strdup(str);// duplica a string original
    strtok(straux, " "); // ignora a primeira palavra (o nome do programa)
    
    while(arg = strtok(NULL, " ")){
        p->arguments[i] = (char*) malloc(strlen(arg) + 1);
        strcpy(p->arguments[i], arg);
        printf("%s\n",p->arguments[i]);
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

    //int fres = 0;
    printf("%d\n", argc);

    if(argc < 4){
        printf("Falta de argumentos!\n");
        return -1;
    }

    else{

        char* option = strdup(argv[1]); // pode ser "execute" ou "status"
        char* flag = strdup(argv[2]);  // pode ser "-u" para programa individual ou "-p" para pipeline de programas
        char* programs = strdup(argv[3]);  // string que contem os programas a executar mais os argumentos
        

        if(strcmp(option, "execute") == 0 && strcmp(flag, "-u") == 0){
            // fazer parse single
            
            //printf("%s",programs);
            Prog* p = parse_single(programs);

            //-----------------------------------------
            // Testar se a estrutura foi de facto criada

            printf("\n");
            printf("Nome: %s\n", p->prog_name);
            printf("Argumentos:\n");
            for (int i = 0; i < p->num_args; i++) {
                printf("  %s\n", p->arguments[i]);
            }
            //-----------------------------------------
            
            int fd = open("FIFO", O_WRONLY);
            if(fd < 0){
                perror("Erro no open!\n");
            }
            
            printf("\n");
            write(fd, &(p->prog_name), strlen(p->prog_name));

            execlp(p->prog_name, p->prog_name, NULL);

            free(p);
                    
            close(fd);

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