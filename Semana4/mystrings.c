#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>                          // Javier Barceló Pérez, Grupo 3.1 - Tarea Semana 4 ASO
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>      // para la función isprint()


#define DEFAULT_BUFSIZE 1024    // tamaño del buffer por defecto
#define DEFAUTL_MINLENGTH 4     // longitud minima por defecto


int main(int argc, char *argv[])
{
    int opt, bufsize = DEFAULT_BUFSIZE, minlength = DEFAUTL_MINLENGTH;

    if (argc<2) // no hay argumentos
    {
        fprintf(stderr, "USO: ./%s [-t BUFSIZE] [-n MINLENGTH]\nLee de la entrada estándar el flujo de bytes recibido y escribe en la salida estándar las cadenas compuestas por caracteres imprimibles incluyendo espacios, tabuladores y saltos de línea, que tengan una longitud mayor o igual a un tamaño dado.\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    optind = 1;
    while ((opt = getopt(argc, argv, "t:n:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            minlength = atoi(optarg);
            break;
        case 't':
            bufsize = atoi(optarg);
            break;
        default:
            fprintf(stderr, "USO: ./%s [-t BUFSIZE] [-n MINLENGTH]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    //comprobar que los argumentos son correctos
    if (minlength < 1 || minlength > 255)
    {
        fprintf(stderr, "ERROR: La longitud mínima debe estar entre 1 y 255\n");
        exit(EXIT_FAILURE);
    }

    if (bufsize < minlength || bufsize < 1048577)
    {
        fprintf(stderr, "ERROR: El tamaño del buffer debe ser mayor o igual que la longitud mínima y menor de 1MB.\n");
        exit(EXIT_FAILURE);
    }

    //abrir descriptores no hace falta porque se usa la saldia estandar

    // ejecucion

    ssize_t num_read, num_left, num_written;
    char *bufin = malloc(bufsize * sizeof(char));
    char *bufout = malloc(bufsize * sizeof(char));

    while ((num_read = read(STDIN_FILENO, bufin, bufsize)) > 0)
    {
        
        
        
        // if (num_written == -1)
        // {
        //     perror("write(fdin)");
        //     exit(EXIT_FAILURE);
        // }
    }

    if (num_read == -1)
    {
        perror("read(fdin)");
        exit(EXIT_FAILURE);
    }
    //liberar memoria
    free(bufin);
    free(bufout);

    return EXIT_SUCCESS;
}

