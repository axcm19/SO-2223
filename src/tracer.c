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


Prog* parse_single(char* input){
    // faz o parse de uma string que representa um unico programa 

    /*
    char* line = NULL;
    ssize_t read;
    size_t len; // Será definido pela funçao getline quando alocar espaço para a string
    Prog* p = malloc(sizeof(struct prog));
    int res = 0;
    */
    Prog* p = malloc(sizeof(struct prog));

    //char *found;

    //found = strtok(input," ");

    char s[2] = " "; 
    char *token;

    token = strtok(input,s);
    p->pid = 1;
    p->prog_name = strdup(token);

    int i = 0;

    while( token != NULL ) {
        printf( " %s\n", token );
        //p->arguments[i] = strdup(token);

        token = strtok(NULL, s);
        i++;
    }

    /*
    if(found == NULL){
        printf("\t'%s'\n",input);
        puts("\tERRO! - No program found");
        return NULL;
    }

    while(found){
        //printf("\t'%s'\n",found);

        int execution = execlp(found, found, NULL);
        printf("TERMINEI! %d\n", execution);
        sleep(1);
        
        p->prog_name = strdup(found);
        p->arguments = NULL;
        
        found = strtok(NULL," ");

    }
    */

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
            
            Prog* p = malloc(sizeof(struct prog));
            p = parse_single(programs);

            
            int fd = open("FIFO", O_WRONLY);
            if(fd < 0){
                perror("Erro no open!\n");
            }
            

            //int res;
            
            
            write(fd, &(p->prog_name), sizeof(p->prog_name));

            execlp(p->prog_name, p->prog_name, NULL);
                    
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