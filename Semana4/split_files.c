#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>                          // Javier Barceló Pérez, Grupo 3.1 - Tarea Semana 4 ASO
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#define DEFAULT_BUFSIZE 1024

int main(int argc, char *argv[])
{
    int opt, bufsize = DEFAULT_BUFSIZE;
    
    
    if (argc < 2){ // si no hay argumentos
        fprintf(stderr, "ERROR: No hay ficheros de salida.\n");
        fprintf(stderr, "Uso: %s [-t BUFSIZE] FILEOUT1 [FILEOUT2 ... FILEOUTn]\nNDivide en ficheros el flujo de bytes que recibe por la entrada estandar.\nEl numero maximo de ficheros de salida es 16.\n -t BUFSIZE  Tamaño de buffer donde 1 <= BUFSIZE <= 1MB (por defecto 1024).\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    optind = 1;
    while ((opt = getopt(argc, argv, "t:")) != -1)
    {
        if (opt == 't')
        {
            bufsize = atoi(optarg);
        }
        else
        {
            fprintf(stderr, "Uso: %s [-t BUFSIZE] FILEOUT1 [FILEOUT2 ... FILEOUTn]\nNDivide en ficheros el flujo de bytes que recibe por la entrada estandar.\nEl numero maximo de ficheros de salida es 16.\n -t BUFSIZE  Tamaño de buffer donde 1 <= BUFSIZE <= 1MB (por defecto 1024).\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    //  si no hay archivos de salida error
    if (argc == optind)
    {
        fprintf(stderr, "Error: No hay ficheros de salida.\n");
        fprintf(stderr, "Uso: %s [-t BUFSIZE] [-o FILEOUT] FILEIN1 [FILEIN2 ... FILEIN16]\nNo admite lectura de la entrada estandar.\n -t BUFSIZE  Tamaño de buffer donde 1 <= BUFSIZE <= 128MB\n -o FILEOUT  Usa FILEOUT en lugar de la salida estandar\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int ficheros = argc - optind;
    // en caso de que haya más de 16 ficheros de salida
    if (ficheros > 16)
    {
        fprintf(stderr, "Error: Demasiados ficheros de salida.\n");
        fprintf(stderr, "Uso: %s [-t BUFSIZE] [-o FILEOUT] FILEIN1 [FILEIN2 ... FILEIN16]\nNo admite lectura de la entrada estandar.\n -t BUFSIZE  Tamaño de buffer donde 1 <= BUFSIZE <= 128MB\n -o FILEOUT  Usa FILEOUT en lugar de la salida estandar\n", argv[0]);
        exit(EXIT_FAILURE); 
    }



    return EXIT_SUCCESS;
}
