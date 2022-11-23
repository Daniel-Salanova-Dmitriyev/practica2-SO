#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#define DEBUGN1 1

char const PROMPT = '$';



int internal_cd(char **args){
    #if DEBUGN1
        printf("Comando que nos permitirá cambiar de directorio");
    #endif
    return 1;
}

int internal_export(char **args){
    #if DEBUGN1
        printf("Comando que define una variable de entorno");
    #endif
    return 1;
}

int internal_source(char **args){
    #if DEBUGN1
        printf("Comando que hace que un proceso se ejecute sin crear un hijo");
    #endif
    return 1;
}

int internal_jobs(char **args){
    #if DEBUGN1
        printf("Comando que nos muestra los procesos resultantes de nuestro terminal ");
    #endif
    return 1;
}

int internal_fg(char **args){
    #if DEBUGN1
        printf("Comando que mueve un proceso en segundo plano al primer plano");
    #endif
    return 1;
}

int internal_bg(char **args){
    #if DEBUGN1
        printf("Comando que reanuda un proceso que esta suspendido en segundo plano");
    #endif
    return 1;
}


void imprimir_prompt(){
    char *usuario = getenv("USER");
    char *direccion = getenv("PWD");

    printf(ROJO_T "%s:" AZUL_T "%s" BLANCO_T "%c ", usuario, direccion, PROMPT);    
}


char *read_line(char *line){
    imprimir_prompt();
    int n = COMMAND_LINE_SIZE;

    
    char *linea;
  
    linea = fgets(line,n,stdin); //LEEMOS UNA LINEA DE LA CONSOLA
    fflush(stdout);
    if(linea == NULL && feof(stdin)){ //SI
        printf("\n \r");
        printf(GRIS_T "Se va ha cerrar la terminal\n");
        exit(0);
    }

    if(linea != NULL){
        //Colocamos un \0 al final de la linea
        int longitud = strlen(linea);
        linea[longitud-1] = '\0'; //Ponemos a null la \n
    }
    return linea;
}



int check_internal(char **args){
    int retorno;
    retorno = strcmp(args[0],"cd");//internal_cd() 
    if (retorno == 0){
        internal_cd(args);
        return 1;
    }
    retorno = strcmp(args[0],"export");//internal_export()
    if (retorno == 0){
        internal_export(args);
        return 1;
    }
    retorno = strcmp(args[0],"source");//internal_source() 
    if (retorno == 0){
        internal_source(args);
        return 1;
    }
    retorno = strcmp(args[0],"jobs");//internal_jobs(),
    if (retorno == 0){
        internal_jobs(args);
        return 1;
    }
    retorno = strcmp(args[0],"fg");//internal_fg()
    if (retorno == 0){
        internal_fg(args);
        return 1;
    }
    retorno = strcmp(args[0],"bg");//internal_bg()
    if (retorno == 0){
        internal_bg(args);
        return 1;
    }
    retorno = strcmp(args[0],"exit");// exit()
    if (retorno == 0){
        exit(0);
        return 1;
    }
    return 0;
}


int parse_args(char **args, char *line){
    char *sep = "\t\n\r ";
    //char *sep = " ";
    //*args = strtok(line, sep);
    char *token = strtok(line,sep);
    int i = 0;

    while (token != NULL)
    {
        
        if (token[0] == '#')
        {
            args[i] = NULL;
        }
        args[i] = token;
        i++;
        token = strtok(NULL, sep);
    }
  
    return i;
}


int execute_line(char *line){
    char **args = malloc(ARGS_SIZE);
    parse_args(args, line);
   
    check_internal(args);
   
    free(args); //LIberamos el espacio alojado por los argumentos

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
         free(line); //liberamos el espacio alojado por la linea
    }
    
}
