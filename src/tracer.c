#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>


//################################################################################################################################


typedef struct prog{
    int pid;
    char* prog_name;
    char** arguments;
} Prog;


//################################################################################################################################


void parse_single(char* input){
    // faz o parse de uma string que representa um unico programa 

    /*
    char* line = NULL;
    ssize_t read;
    size_t len; // Será definido pela funçao getline quando alocar espaço para a string
    Prog* p = malloc(sizeof(struct prog));
    int res = 0;
    */

    char *found;

    found = strtok(input," ");

    if(found == NULL){
        printf("\t'%s'\n",input);
        puts("\tERRO! - No program found");
        return ;
    }

    while(found){
        //printf("\t'%s'\n",found);
        int fres = fork();

        if(fres == 0){
                    // codigo do filho

                    int son_id = getpid();
                    int execution = execlp(found, found, NULL);
                    printf("TERMINEI! %d\n", execution);
                    sleep(1);
                    _exit(son_id);
                    //found = strtok(NULL," ");

        }

        
        int status;
        wait(&status);

        if(WEXITSTATUS(status)){
            printf("\nTERMINEI! %d\n", WEXITSTATUS(status));
        }
        else{
            printf("\nCORREU MAL! %d\n", status);
        }  

        found = strtok(NULL," ");

    }

    /*
    while ((read = getline(&line, &len, input)) != -1) {

        //-------------------------------------------------
        //ESTE PEDAÇO DE CODIGO COLOCA A INFORMAÇÃO EXTRAIDA
        //DA LINHA NUMA ESTRUTURA DE PROG

        char* token = NULL;
        char *line2 = line;
        int i = 0;
        char* arr[7];
        while ((token = strsep(&line2, " ")) != NULL){
            arr[i] = token;
            i++;
        }

        
        //p->prog_name = strdup(arr[0]);
        //p->arguments = strdup(arr[1]);
        res = execlp(arr[0], arr[0], NULL);

        if(res > 0){
            printf("%s", arr[0]);
        }
    }
        
    free(line); // É preciso libertar a memória alocada
    */
    

}


//################################################################################################################################


int parse_pipeline(){
    return 0;
}


//################################################################################################################################


int main(int argc, char **argv) {

    //int fres = 0;
    printf("%d\n", argc);

    if(argc < 3){
        printf("Falta de argumentos!\n");
        return -1;
    }

    else{

        char* option = strdup(argv[1]); // pode ser "execute" ou "status"
        char* flag = strdup(argv[2]);  // pode ser "-u" para programa individual ou "-p" para pipeline de programas
        char* programs = strdup(argv[3]);  // string que contem os programas a executar mais os argumentos
        

        if(strcmp(option, "execute") == 0 && strcmp(flag, "-u") == 0){
            // fazer parse single
            parse_single(programs);

            /*
            for(i = 1; i < argc; i++){
            
            // criar um filho para cada argumento
                fres = fork();
                
                if(fres == 0){
                    // codigo do filho

                    int son_id = getpid();
                    printf("TERMINEI! %d\n", 1);
                    sleep(1);
                    _exit(son_id);

                }

            }

            for(i = 1; i < argc; i++){
                int status;
                wait(&status);

                if(WEXITSTATUS(status)){
                    printf("\nTERMINEI! %d\n", WEXITSTATUS(status));
                }
                else{
                    printf("\nCORREU MAL! %d\n", i);
                }
            }*/

        }

        if(strcmp(option, "execute") == 0 && strcmp(flag, "-p") == 0){
            // fazer parse do pipe
        }


    }
        
    return 0;

}


//################################################################################################################################