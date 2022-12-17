//Creadores
//Arkadiy Kosyuk
//Alexander Cordero Gómez
//Daniel Salanova Dmitriyev

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
#define DEBUGN1 0
#define DEBUGN2 1
#define DEBUGN3 0
#define DEBUGN4 0
#define DEBUGN5 0
#define DEBUGN6 0


char const PROMPT = '$'; 
int setenv(const char *name, const char *value, int overwrite);
int chdir(const char *path);
char *getcwd(char *buffer,int maxlen);
char *read_line(char *line);
int execute_line(char *line);
int parse_args(char **args, char *line);
int check_internal(char **args);
int internal_cd(char **args);
int internal_export(char **args);
int internal_source(char **args);
int internal_jobs(char **args);
int internal_fg(char **args);
int internal_bg(char **args);
int n_pids = 1; 

char *eliminarCaracter(char *linea, char caracter){
    int index = 0;
    int new_index = 0;

    while (linea[index])
    {
        if (linea[index] != caracter)
        {
            linea[new_index] = linea[index];
            new_index++;
        }

        index++;
    }

    linea[new_index] = '\0';
    return linea;
}

/**
 * Función que permite la navegación en los directorios
*/
int internal_cd(char **args){
    if(args[1] != NULL){ //Si tenemos al menos 1 argumento
        
        char *ruta; //Dirección a desplazarnos
        
        char *linea =  malloc(sizeof(char) * COMMAND_LINE_SIZE);
        if(!linea){
            perror("Error: ");
        }
        for(int i = 0; args[i];i++){
            strcat(linea, " ");
            strcat(linea, args[i]);
        }
        //Código ASCII para comillas->34, comilla->39, barra->92
        if(args[1][0] == 34){ //CASO COMILLAS
            ruta = strchr(linea, 34);
            ruta = eliminarCaracter(ruta,34);   
        }else if (args[1][0] == 39){ //CASO COMILLA
            ruta = strchr(linea, 39);
            ruta = eliminarCaracter(ruta,39);
        }else if (args[1][strlen(args[1])-1] == 92){ //CASO BARRA
            ruta = strchr(linea, args[1][0]);
            ruta = eliminarCaracter(ruta,92);
        }else{
            ruta = args[1];
        }
        
        if( chdir(ruta) != 0){   //Cambiamos de dirección
            printf(ROJO_T"No existe la direccion\n");
        }else{
            char *dir = malloc(COMMAND_LINE_SIZE);
            if(!dir){
                perror("Error: ");
            }
            getcwd(dir, COMMAND_LINE_SIZE);


            #if DEBUGN2
                printf(GRIS_T "DIreccion actual: %s \n", dir);
            #endif

            //Actualizamos el prompt PWD->dirección actual
            setenv("PWD", dir,1);
            free(dir);
        }
        free(linea);
    }else{ //SI no tenemos argumento

        chdir(getenv("HOME")); //Cambiamos de dirección
    
        char *dir = getenv("HOME");
        getcwd(dir, COMMAND_LINE_SIZE);

        #if DEBUGN2
            printf(GRIS_T "DIreccion actual: %s \n", dir);
        #endif

        //Actualizamos el prompt PWD->dirección actual
        setenv("PWD", dir,1);
    }

     return EXIT_SUCCESS;
}

/**
 * Función que cambia el valor de la variables de entorno
*/
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
    valor = strtok(NULL, "");
    
    #if DEBUGN2 
        printf(GRIS_T "Variable Inicial: %s\n", getenv(nombre));
    #endif
	//si sintaxis correcta
    if (nombre && valor){
		//se cambia el valor de la variable de entorno
        setenv(nombre, valor, 1);

        #if DEBUGN2
            printf(GRIS_T "Variable de entorno ACTUALIZADA: %s\n", getenv(nombre));
        #endif
    }else{
        fprintf(stderr, ROJO_T "export: Sintaxis incorrecta\n");
    }
 
    return 1;
}

//implementacion de la funcion internal source
int internal_source(char **args)
{
    #if DEBUGN1
        printf("Comando que hace que un proceso se ejecute sin crear un hijo\n");
    #endif
    return 1;
}

int internal_jobs(char **args){
    #if DEBUGN1
        printf("Comando que nos muestra los procesos resultantes de nuestro terminal\n");
    #endif
    return 1;
}

/**
 * Función que pasa a segundo plano un proceso detenido
*/
int internal_bg(char **args){
    #if DEBUGN1
        printf("Comando que reanuda un proceso que esta suspendido en segundo plano\n");
    #endif
    return 1;
}

/**
 * Función que manda al primer plano procesos detenidos o en segundo plano
*/
int internal_fg(char **args){     
    #if DEBUGN1
        printf("Comando que mueve un proceso en segundo plano al primer plano\n");
    #endif
    return 1;
}

void imprimir_prompt(){
    char *usuario = getenv("USER");
    char *direccion = getenv("PWD");

    printf(ROJO_T "%s:" AZUL_T "%s" BLANCO_T "%c ", usuario, direccion, PROMPT); 
    fflush(stdout);
}

/**
 * Método que lee la linea que se escribe en el prompt
*/
char *read_line(char *line){
    imprimir_prompt(); //Imprimimos el prompt  
    fflush(stdout); //Forzamos el vaciodo del buffer de salida

    char *linea = fgets(line,COMMAND_LINE_SIZE,stdin); //LEEMOS UNA LINEA DE LA CONSOLA
    if(linea == NULL && feof(stdin)){
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

/**
 * Función que comprobará si un comando escrito es interno o externo
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

/**
 * Función que se encargará de dividir en argumentos una linea
*/
int parse_args(char **args, char *line)
{   
    char aux[COMMAND_LINE_SIZE];
    strcpy(aux, line);  
    int i = 0;
    char *token = strtok(aux, " \n\r\t");

    while (token != NULL) {
        args[i] = token;
        // Mientras no sea un comentario lo añadimos al array
        if (strncmp(args[i],"#",1) == 0) { 
            break;   
        }
        i++;
        // Ponemos NULL para no sobreescribir
        token = strtok(NULL, " \n\r\t");
    }

    // Agregamos un null al final de la lista de argumentos
    args[i] =  0; 

    //Imprimimos cuales son los tokens y cuantos hay
    #if DEBUGN1 
        int j = 0;
        while(args[j]){
            printf(GRIS_T"Token -> %s\n",args[j]);
            j++;
        }  
        printf(GRIS_T"Numero tokens -> %i\n",j);
    #endif

    // Le quitamos el salto de línea a line
    strtok(line, "\n\r"); 
    return i;    
}

/**
 * Función que se encargará de que se ejcuten tanto comandos internos como externos
*/
int execute_line(char *line){
    
    char **args = malloc(ARGS_SIZE);
    if(!args){
        perror("Error: ");
    }
    char lineaComando[COMMAND_LINE_SIZE];
    
    strcpy(lineaComando,line); 
    parse_args(args, line);
    check_internal(args);

    memset(line, '\0', COMMAND_LINE_SIZE);
    free(args); 
    return EXIT_SUCCESS;
}

/**
 * Funcion main
*/
int main(int argc, char *argv[]){   
    char *line = (char *)malloc(sizeof(char) * COMMAND_LINE_SIZE);
    if(!line){ //En caso de que no se haya asignado bien memoria
        perror("Error: ");
    }

    while(1){
        if(read_line(line)){
            
            execute_line(line);
        }
    }    
    return EXIT_SUCCESS;
}
