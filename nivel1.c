#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_PROMPT_SIZE 60

#define COMMAND_LINE_SIZE 1024
#define ARGS_SIZE 64

#define RESET "\033[0m"
#define NEGRO_T "\x1b[30m"
#define NEGRO_F "\x1b[40m"
#define GRIS_T "\x1b[94m"
#define ROJO_T "\x1b[31m"
#define VERDE_T "\x1b[32m"
#define AMARILLO_T "\x1b[33m"
#define AZUL_T "\x1b[34m"
#define MAGENTA_T "\x1b[35m"
#define CYAN_T "\x1b[36m"
#define BLANCO_T "\x1b[97m"
#define NEGRITA "\x1b[1m"

char const PROMPT = '$';
char *read_line(char *line){
    imprimir_prompt();
    char str[COMMAND_LINE_SIZE];
    int n = COMMAND_LINE_SIZE;


    char *linea = fgets(str,n,stdin); //LEEMOS UNA LINEA DE LA CONSOLA
    if(linea == NULL && feof(stdin)){ //SI
        printf("\n \r");
        printf(GRIS_T "Se va ha cerrar la terminal\n");
        exit(0);
    }

    if(linea != NULL){
        //Colocamos un \0 al final de la linea
        for(int i = 0; i< COMMAND_LINE_SIZE; i++){
            if(linea[i] == '\n'){
                linea[i] == '\0';
            }
        }
    }

    return linea;
}

void imprimir_prompt(){
    char *usuario = getenv("USER");
    char *direccion = getenv("PWD");

    printf(ROJO_T "%s:" AZUL_T "%s" BLANCO_T "%c ", usuario, direccion, PROMPT); 
    fflush(stdout);
}



int execute_line(char *line){
    char *args[ARGS_SIZE];
    parse_args(**args, *line);
    check_internal(**args);
}

int parse_args(char **args, char *line){
char *sep = "\t\n\r ";
    *args = strtok(line, sep);
    int i = 0;
    while (args[i] != NULL)
    {
        if (*args[i] == '#')
        {
            args[i] = NULL;
        }
        i++;
        args[i] = strtok(NULL, sep);
    }
    return i;
}


int check_internal(char **args){
    int retorno;
    retorno = strcmp(args[0],"cd");//internal_cd() 
    if (retorno == 0){
        internal_cd(**args);
        return true;
    }
    retorno = strcmp(args[0],"export");//internal_export()
    if (retorno == 0){
        internal_export(**args);
        return true;
    }
    retorno = strcmp(args[0],"sourcoce");//internal_source() 
    if (retorno == 0){
        internal_source(**args);
        return true;
    }
    retorno = strcmp(args[0],"jobs");//internal_jobs(),
    if (retorno == 0){
        internal_jobs(**args);
        return true;
    }
    retorno = strcmp(args[0],"fg");//internal_fg()
    if (retorno == 0){
        internal_fg(**args);
        return true;
    }
    retorno = strcmp(args[0],"bg");//internal_bg()
    if (retorno == 0){
        internal_bg(**args)
        return true;
    }
    retorno = strcmp(args[0],"exit");// exit()
    if (retorno == 0){
        exit(0);
        return true;
    }
    return false;
}

int internal_cd(char **args){
    printf("Comando que nos permitirá cambiar de directorio");
}

int internal_export(char **args){
    printf("Comando que nos permitirá cambiar de directorio");
}

int internal_source(char **args){

}

int internal_jobs(char **args){

}

int internal_fg(char **args){

}

int internal_bg(char **args){

}




/**
 * MAIN PROVISIONAL
*/
void main(){
    char line[COMMAND_LINE_SIZE];
    while(1){
        if(read_line(line)){
            execute_line(line);
        }
    }
    
}
