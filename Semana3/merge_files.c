#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#define DEFAULT_BUFSIZE 1024


void merge(){

}



int main(int argc, char *argv[])
{
    int opt, bufsize, fdout;
    char *fileout;
    char *bufout;

    bufsize = DEFAULT_BUFSIZE;  // tamaño del buffer por defecto

    if (argc < 2){ // si no hay argumentos
        fprintf(stderr, "Uso: %s [-t BUFSIZE] [-o FILEOUT] FILEIN1 [FILEIN2 ... FILEIN16]\nNo admite lectura de la entrada estandar.\n -t BUFSIZE  Tamaño de buffer donde 1 <= BUFSIZE <= 128MB\n -o FILEOUT  Usa FILEOUT en lugar de la salida estandar\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    optind = 1;
    while ((opt = getopt(argc, argv, "o:t:")) != -1)
    {
        switch (opt)
        {
        case 'o':
            fileout = optarg;
            break;
        case 't':
            bufsize = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Uso: %s [-t BUFSIZE] [-o FILEOUT] FILEIN1 [FILEIN2 ... FILEIN16]\nNo admite lectura de la entrada estandar.\n -t BUFSIZE  Tamaño de buffer donde 1 <= BUFSIZE <= 128MB\n -o FILEOUT  Usa FILEOUT en lugar de la salida estandar\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    //si no hay arvhivos de entrada error
    if (argc == optind)
    {
        fprintf(stderr, "Error: No hay ficheros de entrada.\n");
        fprintf(stderr, "Uso: %s [-t BUFSIZE] [-o FILEOUT] FILEIN1 [FILEIN2 ... FILEIN16]\nNo admite lectura de la entrada estandar.\n -t BUFSIZE  Tamaño de buffer donde 1 <= BUFSIZE <= 128MB\n -o FILEOUT  Usa FILEOUT en lugar de la salida estandar\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // si hay más de 16 ficheros de entrada
    if ((argc-optind)>16)
    {
        fprintf(stderr, "Error: Demasiados ficheros de entrada. Máximo 16.\n");
        fprintf(stderr, "Uso: %s [-t BUFSIZE] [-o FILEOUT] FILEIN1 [FILEIN2 ... FILEIN16]\nNo admite lectura de la entrada estandar.\n -t BUFSIZE  Tamaño de buffer donde 1 <= BUFSIZE <= 128MB\n -o FILEOUT  Usa FILEOUT en lugar de la salida estandar\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Abre el fichero de salida si ha sido indicado
    if (fileout != NULL)
    {
        fdout = open(fileout, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (fdout == -1)
        {
            perror("open(fileout)");
            exit(EXIT_FAILURE);
        }
    }
    else // si no se indica fichero de salida, se usa la salida estandar
        fdout = STDOUT_FILENO;

    // Comprueba que el bufsize es mayor de 1 byte y menos de 128MB
    if (bufsize < 1 || bufsize > 134217728)
    {
        fprintf(stderr, "Error: Tamaño de buffer incorrecto.\n\n");
        fprintf(stderr, "Uso: %s [-t BUFSIZE] [-o FILEOUT] FILEIN1 [FILEIN2 ... FILEIN16]\nNo admite lectura de la entrada estandar.\n -t BUFSIZE  Tamaño de buffer donde 1 <= BUFSIZE <= 128MB\n -o FILEOUT  Usa FILEOUT en lugar de la salida estandar\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    // EJECUCIÓN - TENEMOS AL MENOS UN FICHERO DEL CUAL LEER BYTE A BYTE
    
    
    // hay que abrir los ficheros de entrada


    return EXIT_SUCCESS;
}

