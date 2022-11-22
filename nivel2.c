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
    int n = COMMAND_LINE_SIZE;

    fflush(stdout);
    char *linea = malloc(COMMAND_LINE_SIZE);
  
    linea = fgets(line,n,stdin); //LEEMOS UNA LINEA DE LA CONSOLA
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

void imprimir_prompt(){
    char *usuario = getenv("USER");
    char *direccion = getenv("PWD");

    printf(ROJO_T "%s:" AZUL_T "%s" BLANCO_T "%c ", usuario, direccion, PROMPT); 
   
}



int execute_line(char *line){
    char **args = malloc(ARGS_SIZE);
    parse_args(args, line);
    check_internal(args);     
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

/**
 *  while (args[i] != NULL)
    {
        if (*args[i] == '#')
        {
            args[i] = NULL;
        }
        i++;
        args[i] = strtok(NULL, sep);
    }
    return i;
*/


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

int internal_cd(char **args){
    if(args[1] != NULL){
        chdir(args[1]); //Cambiamos de dirección
        
        //A modo de test
        char *dir = malloc(COMMAND_LINE_SIZE);
        getcwd(dir, COMMAND_LINE_SIZE);
        printf(GRIS_T "DIreccion actual: %s \n", dir);

        //Actualizamos el prompt PWD->dirección actual
        setenv("PWD", dir,1);
    }

     return EXIT_SUCCESS;
}

int internal_export(char **args){
   
   
    //Función que separa en tokens el argumento NOMBRE=VALOR
    //Inicializamos las diferentes variables a utilizar
     char *separacion = "=";
    char *nombre;
    char *valor;

    if(args[1] == NULL){ //EN el caso de que no se le pase la variable a actualizar
        fprintf(stderr, ROJO_T "export: Sintaxis incorrecta\n");
        return 1;
    }
    
    //Primer token
    nombre = strtok(args[1], separacion);
    //Segundo token
    valor = strtok(NULL, separacion);
    
    printf(GRIS_T "Variable Inicial: %s\n", getenv(nombre));
    
	//si sintaxis correcta
    if (nombre && valor){
		//se cambia el valor de la variable de entorno
        setenv(nombre, valor, 1);
        printf(GRIS_T "Variable de entorno ACTUALIZADA: %s\n", getenv(nombre));
    }else{
        fprintf(stderr, ROJO_T "export: Sintaxis incorrecta\n");
    }
 
    return 1;
}

int internal_source(char **args){
     printf("Comando que hace que un proceso se ejecute sin crear un hijo");
}

int internal_jobs(char **args){
     printf("Comando que nos muestra los procesos resultantes de nuestro terminal ");
}

int internal_fg(char **args){
     printf("Comando que mueve un proceso en segundo plano al primer plano");
}

int internal_bg(char **args){
     printf("Comando que reanuda un proceso que esta suspendido en segundo plano");
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
