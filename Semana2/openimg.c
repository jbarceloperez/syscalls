#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char *argv[])
{
    pid_t pid, wpid;
    int status = 0;

    if (argc < 2){ // si no hay argumentos
        fprintf(stderr, "USO: ./openimg -v VISOR [IMGs]\n");
        return EXIT_FAILURE;
    }

    int e = strcmp(argv[1],"-v");
    if (strcmp(argv[1],"-v") > 0 || argc < 3){     //si no se especifica -v VISOR se termina la ejecucion
        fprintf(stderr, "USO: ./openimg -v VISOR [IMGs]\n");
        return EXIT_FAILURE;
    }

    char* visor = argv[2];


    if (argc < 4){
        fprintf(stderr, "Error: No hay imágenes que visualizar.\n");
        return EXIT_FAILURE;
    }

    for (int i = 4; i <= argc; i++)
        if ((pid = fork()) == 0) {
            execlp(visor, visor, argv[i], NULL);
            fprintf(stderr, "ERROR: El visor no existe o no está instalado.\n");      //CODIGO DEL HIJO
            return EXIT_FAILURE;
                   
        }

    while ((wpid = wait(&status)) > 0); //PADRE con un while espera a todos los hijos?
    
    return EXIT_SUCCESS;
}


/*
switch (pid=fork()) {
        case -1:    // error
            perror("fork()");
            exit(EXIT_FAILURE);
            break;
    
        case 0:     // proceso hijo
            printf("Hijo\n");
            break;

        default:    // proceso padre
            while ((wpid = wait(&status)) > 0); // con un while espera a todos los hijos?
            printf("Padre\n");
            break;
        }
*/