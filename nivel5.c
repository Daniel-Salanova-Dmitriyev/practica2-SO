#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>

#define LINE_PROMPT_SIZE 60

#define COMMAND_LINE_SIZE 1024
#define ARGS_SIZE 64
#define N_JOBS 64

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
#define DEBUGN2 0
#define DEBUGN3 0
#define DEBUGN4 0
#define DEBUGN5 1


char const PROMPT = '$';
int chdir(const char *path); 
long getcwd(char *buf, unsigned long size);
char *read_line(char *line);
int execute_line(char *line);
int n_pids = 0; 


struct info_job {
   pid_t pid;
   char status; // ‘N’, ’E’, ‘D’, ‘F’ (‘N’: ninguno, ‘E’: Ejecutándose y ‘D’: Detenido, ‘F’: Finalizado) 
   char cmd[COMMAND_LINE_SIZE]; // línea de comando asociada
};

static struct info_job jobs_list [N_JOBS];
static char mi_shell[COMMAND_LINE_SIZE];


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
   
    printf("MI export!!\n");
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

//implementacion de la funcion internal source
int internal_source(char **args)
{

    char *nombre = args[1];
    char *linea = malloc(COMMAND_LINE_SIZE);
    //Comprobamos que haya un nombre
    if (nombre!=NULL)
    {
        //abrimos el fichero con el nombre
        FILE *archivo;
        archivo = fopen(nombre, "r");
        //Comprobamos si es NULL en tal caso seria error
        if (archivo==NULL)
        {

            fprintf(stderr, "source: Sintaxis incorrecta\n");
            
        }
        //caso contrario
        else
        {
            //leemos el fichero mientras no llegues al final o haya error seguira leyendo
            while (fgets(linea, COMMAND_LINE_SIZE, archivo)!=NULL)
            {
                //antes de ejecutar la linea 
                //realizamos un fflush del stream del archivo
                fflush(archivo);
		    //ejecutamos la linea leida
                execute_line(linea);
            }
            fclose(archivo);
        }
    }
    else
    {
        fprintf(stderr, "source: Sintaxis incorrecta\n");
    }
}

int internal_jobs(char **args){
     for(int i = 1; i<n_pids;i++){
        printf("[%i] PID: %i     CMD: %s    STATUS: %c \n", i,jobs_list[i].pid, jobs_list[i].cmd, jobs_list[i].status);
     }
}

int internal_fg(char **args){
     printf("Comando que mueve un proceso en segundo plano al primer plano");
}

int internal_bg(char **args){
     printf("Comando que reanuda un proceso que esta suspendido en segundo plano");
}


void imprimir_prompt(){
    char *usuario = getenv("USER");
    char *direccion = getenv("PWD");

    printf(ROJO_T "%s:" AZUL_T "%s" BLANCO_T "%c ", usuario, direccion, PROMPT); 
   
}

int jobs_list_find(pid_t pid){
    for(int i = 0; i<n_pids; i++){
        if(jobs_list[i].pid == pid){
            return i;
        }
    }
    //Nunca llegará a este punto
    return -1;
}


char *read_line(char *line){
    imprimir_prompt();
    int n = COMMAND_LINE_SIZE;

    fflush(stdout);
    char *linea;
  
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



//manejador para la señal SIGCHLD
void reaper(int signum)
{
    
    signal(SIGCHLD, reaper);
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        printf("Poceso hijo dentro de reaper: %i \n",pid);
        if (pid == jobs_list[0].pid){
            
            //Actualizamos los valores del primer proceso 
            jobs_list[0].pid = 0;
            jobs_list[0].status = 'F';
            strcpy(jobs_list[0].cmd, "\0");
            printf("El pid del proceso terminado %d y el estatus es %d\n", pid, status);

            
        } else{     
            int i = jobs_list_find(pid);
            fprintf(stderr, "El proceso %d ha terminado\n", jobs_list[i].pid);
            jobs_list_remove(i);
        }
    }
}
//elimina un trabajo del array de trabajos 
int jobs_list_remove(int pos) {
    jobs_list[pos].pid = jobs_list[n_pids - 1].pid;
    jobs_list[pos].status = jobs_list[n_pids - 1].status;
    strcpy(jobs_list[pos].cmd,jobs_list[n_pids -1].cmd);
    n_pids--;
    return n_pids;
}
//añade un trabajo al array de trabajos
int jobs_list_add(pid_t pid, char status, char *cmd) {
	//si el numero no es maximo
    if (n_pids < N_JOBS) {
        //añades el pid, status y cmd
        jobs_list[n_pids].pid = pid;
        jobs_list[n_pids].status = status;
        strcpy(jobs_list[n_pids].cmd,cmd);
        n_pids++;//Actualiza el numero de trabajos
    } else
    {
        fprintf(stderr, "Numero máximo de señales alcanzado");
    }
    return n_pids;
}

void ctrlc(int signum){
    signal(SIGINT,ctrlc); //Escuchamos si se pulsa ctrlc

    if(jobs_list[0].pid > 0){ //Hay un proceso en ejecución
        
        if(!(strcmp(jobs_list[0].cmd,mi_shell) == 0)){
            
            if(kill(jobs_list[0].pid,SIGTERM)==0){ //mandamos a acabar dicho proceso
                #if DEBUGN4
                    printf("Se ha enviado señal SIGTERM\n");
                #endif
            }else{
                perror("kill: ");
                exit(-1);
            } 
            
            printf("\n");
            fflush(stdout);

        }else{
            #if DEBUGN4
                printf(ROJO_T "No se puede cerrar el proceso ya que es el minishell\n");
                fflush(stdout);

            #endif
        }
    }else{
            #if DEBUGN4
                printf(ROJO_T"No hay ningún proceso en foreground\n");
                fflush(stdout);
            #endif
    } 
}

void ctrlz(int signum){
    signal(SIGTSTP,ctrlz); // Asignamos a SIGTSTP El manejador ctrlz
    if(jobs_list[0].pid > 0){ // Hay un proceso en ejecución
        if(!(strcmp(jobs_list[0].cmd,mi_shell) == 0)){//Sino es el minishell
        
        //Detenemos el proceso
            if(kill(jobs_list[0].pid,SIGSTOP)==0){ //mandamos el stop dicho proceso
            

                //Lo añadimos al final
                jobs_list_add(jobs_list[0].pid,'D',jobs_list[0].cmd);
                
                //Actualizamos los valores del primer proceso 
                jobs_list[0].pid = 0;
                jobs_list[0].status = 'N';
                strcpy(jobs_list[0].cmd, "\0");

                #if DEBUGN5
                    printf("Se ha enviado señal SIGSTOP\n");
                #endif
                printf("\n");
                fflush(stdout);
            }else{
                perror("kill: ");
                exit(-1);
            } 

        }else{
            #if DEBUGN5
                printf(ROJO_T "No se puede cerrar el proceso ya que es el minishell\n");
                fflush(stdout);

            #endif
        }
    }else{
            #if DEBUGN4
                printf(ROJO_T"No hay ningún proceso en foreground\n");
                fflush(stdout);
            #endif
    } 
}

int execute_line(char *line){
    char **args = malloc(ARGS_SIZE);
    parse_args(args, line);
    int internal = check_internal(args);  
    int background = is_background(args);
    if(!internal){ //si no es interno 
        pid_t pid;
        pid = fork(); //Creamos un hijo
        if(pid > 0){ //padre
            signal(SIGINT, ctrlc);//Asociar el manejador ctrlc a la señal SIGINT.
            signal(SIGCHLD,reaper);//ASociamos el manejador reaper a la señal SIGCHILD
            signal(SIGTSTP, ctrlz);//Asociar el manejador ctrlz a la señal SIGTSTP.
            if(background==0){//comando foreground 
                jobs_list[0].status = 'E';
                jobs_list[0].pid = pid;
                strcpy(jobs_list[0].cmd,line);
               
                while(jobs_list[0].pid  > 0){//Esperando al hijo
                    pause();
                }
                
            }else{ //comando background
                jobs_list_add(jobs_list[0].pid, jobs_list[0].status, jobs_list[0].cmd);
            }   
        }else{ //hijo
            signal(SIGCHLD,SIG_DFL); //Accion por defecto
            signal(SIGINT, SIG_IGN);//Ingnoramos SIGINT
            signal(SIGTSTP, SIG_IGN);//Ingnoramos SIGTSTP 
            if (execvp(args[0], args)){//ejecutar el comando externo solicitado. 
                perror("La ejecución del comando ha fallado\n");
                exit(-1);
            }
            
        }
    }
    free(args); 
}

int is_background(char **args){
    for(int i = 0; i<sizeof(args)/sizeof(args[0]);i++){
        if(strcmp("&",args[i]) == 0){
            *args[i] = NULL;
            return 1;
        }
    }
    return 0;
}

/*
//char *sep = "\t\n\r ";
    //char *token = strtok(args,sep);
    int i = 0;
    while (token != NULL){         
        if (token[i] == '&'){
            args[i] = NULL;//Sustituir el token & por NULL
            //execvp(args[0],args);
            return 1;//token & encontrado
        }
        //args[i] = token;
        //i++;
        //token = strtok(NULL, sep);
    }  
    return 0;////token & no encontrado



*/


/**
 * MAIN PROVISIONAL
*/
void main(int argc, char *argv[]){
    
    //Primer proceso
    struct info_job *proceso = malloc(sizeof(struct info_job));
    memset(proceso->cmd,'\0',sizeof(char)*COMMAND_LINE_SIZE);
    proceso->pid = 0;
    proceso->status= 'N';
    //Inicializamos el job_list 
    jobs_list[0] = *proceso;
    //Recogemos el nombre
    strcpy(mi_shell, argv[0]);

    /*int i = 0;
    while (mi_shell[i])
    {
        i++;
    }
    //añadimos un salto de linea al final
    mi_shell[i] = '\n';
    */
    
    //Añadimos escuchas a las señales SIGCHLD Y SIGINT
    signal(SIGCHLD,reaper);
    signal(SIGINT,ctrlc);
    signal (SIGTSTP, ctrlz);

    char line[COMMAND_LINE_SIZE];
    while(1){
        if(read_line(line)){
            execute_line(line);
        }
    }
    
}
