#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>                          // Javier Barceló Pérez, Grupo 3.1 - Tarea Semana 4 ASO
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#define DEFAULT_BUFSIZE 1024    // tamaño del buffer por defecto
#define DEFAULT_MINLENGTH 4     // longitud minima por defecto para el mystrings

int main(int argc, char *argv[])
{
    int opt, bufsize = DEFAULT_BUFSIZE, minlength = DEFAULT_MINLENGTH;
    char *inputs;

    optind = 1;
    while ((opt = getopt(argc, argv, "t:i:n:")) != -1)
    {
        switch (opt)
        {
        case 't':
            bufsize = atoi(optarg);
            break;
        
        case 'n':
            minlength = atoi(optarg);
            break;
        
        case 'i':
            inputs = optarg;
            break;
        
        default:
            fprintf(stderr, "Uso: ./merge_mystrings_split [-t BUFSIZE] [-n MIN_LENGTH] -i FILEIN1[,FILEIN2,...,FILEINn] FILEOUT1 [FILEOUT2 ...]\n  -t BUFSIZE     Tamaño de buffer donde 1 <= BUFSIZE <= 1MB (por defecto 1024).\n  -n MINLENGTH   Longitud mínima de la cadena. Mayor que 0 y menor que 256 (por defecto 4).\n  -i             Lista de ficheros de entrada separados por comas.\n");
            exit(EXIT_FAILURE);
        }
    }

    // comprobar el tamaño del buffer
    if (bufsize < 1 || bufsize > 1048576)
    {
        fprintf(stderr, "Error: Tamaño de buffer incorrecto.\n\n");
        fprintf(stderr, "Uso: ./merge_mystrings_split [-t BUFSIZE] [-n MIN_LENGTH] -i FILEIN1[,FILEIN2,...,FILEINn] FILEOUT1 [FILEOUT2 ...]\n  -t BUFSIZE     Tamaño de buffer donde 1 <= BUFSIZE <= 1MB (por defecto 1024).\n  -n MINLENGTH   Longitud mínima de la cadena. Mayor que 0 y menor que 256 (por defecto 4).\n  -i             Lista de ficheros de entrada separados por comas.\n");
        exit(EXIT_FAILURE);
    }

    //comprobar la longitud minima
    if (minlength < 1 || minlength > 255)
    {
        fprintf(stderr, "ERROR: La longitud mínima debe estar entre 1 y 255\n");
        fprintf(stderr, "Uso: ./merge_mystrings_split [-t BUFSIZE] [-n MIN_LENGTH] -i FILEIN1[,FILEIN2,...,FILEINn] FILEOUT1 [FILEOUT2 ...]\n  -t BUFSIZE     Tamaño de buffer donde 1 <= BUFSIZE <= 1MB (por defecto 1024).\n  -n MINLENGTH   Longitud mínima de la cadena. Mayor que 0 y menor que 256 (por defecto 4).\n  -i             Lista de ficheros de entrada separados por comas.\n");
        exit(EXIT_FAILURE);
    }


    return EXIT_SUCCESS;
}
