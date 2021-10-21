#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>                          // Javier Barceló Pérez, Grupo 3.1 - Tarea Semana 4 ASO
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>      // para la función isprint()
#include <string.h>     // para el strcpm()


#define DEFAULT_BUFSIZE 1024    // tamaño del buffer por defecto
#define DEFAUTL_MINLENGTH 4     // longitud minima por defecto

void escribir(int count, char *buf)
{
    size_t num_left, num_written;
    char *buf_left;

    num_left = count;
    buf_left = buf;
    //escrituras
    while((num_left) > 0 && ((num_written = write(STDOUT_FILENO, buf_left, num_left)) > 0))
    {
        num_left -= num_written;
        buf_left += num_written;
    }
    if (num_written == -1)
    {
        perror("write(fdin)");
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char *argv[])
{
    int opt, bufsize = DEFAULT_BUFSIZE, minlength = DEFAUTL_MINLENGTH;

    // if (argc<2) // no hay argumentos
    // {
    //     fprintf(stderr, "USO: %s [-t BUFSIZE] [-n MINLENGTH]\nLee de la entrada estándar el flujo de bytes recibido y escribe en la salida estándar las cadenas compuestas por caracteres imprimibles incluyendo espacios, tabuladores y saltos de línea, que tengan una longitud mayor o igual a un tamaño dado.\n  -t BUFSIZE     Tamaño de buffer donde MINLENGTH <= BUFSIZE <= 1MB (por defecto 1024).\n  -n MINLENGTH   Longitud mínima de la cadena. Mayor que 0 y menor que 256 (por defecto 4).\n", argv[0]);
    //     exit(EXIT_FAILURE);
    // }

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
            fprintf(stderr, "USO: %s [-t BUFSIZE] [-n MINLENGTH]\nLee de la entrada estándar el flujo de bytes recibido y escribe en la salida estándar las cadenas compuestas por caracteres imprimibles incluyendo espacios, tabuladores y saltos de línea, que tengan una longitud mayor o igual a un tamaño dado.\n  -t BUFSIZE     Tamaño de buffer donde MINLENGTH <= BUFSIZE <= 1MB (por defecto 1024).\n  -n MINLENGTH   Longitud mínima de la cadena. Mayor que 0 y menor que 256 (por defecto 4).\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    //comprobar que los argumentos son correctos
    if (minlength < 1 || minlength > 255)
    {
        fprintf(stderr, "ERROR: La longitud mínima debe estar entre 1 y 255\n");
        exit(EXIT_FAILURE);
    }

    if (bufsize < minlength || bufsize > 1048576)
    {
        fprintf(stderr, "ERROR: El tamaño del buffer debe ser mayor o igual que la longitud mínima y menor de 1MB.\n");
        exit(EXIT_FAILURE);
    }

    //abrir descriptores no hace falta porque se usa la saldia estandar

    // ejecucion

    ssize_t num_read, num_left, num_written;
    char *bufin = malloc(bufsize * sizeof(char));
    char *bufout = malloc(bufsize * sizeof(char));
    char *buf_left;
    int count = 0;  //lleva la cuenta de cuanto se ha escrito en el buffer de escritura

    int fd = open("in", O_RDONLY);//DEBUG
    while ((num_read = read(fd, bufin, bufsize)) > 0)
    {
        for (int i = 0; i < num_read; i++)
        {
            if (isprint(bufin[i]) || bufin[i] == 9 || bufin[i] == 10)  //si el caracter es imprimible, es un tabulador (\t) o es un salto de línea (\n)
            {
                bufout[count] = bufin[i];   //se añade al buffer de escritura 
                count++;
                if (count == bufsize)   // si se ha lenado el buffer de escritura, escribe por salida estandar
                {
                    escribir(count, bufout);
                    count = 0;
                }
            }
        }
    }

    if (num_read == -1)
    {
        perror("read(fdin)");
        exit(EXIT_FAILURE);
    }

    //en caso de que no se haya llenado el buffer,comprobar que la longitud de caracteres imprimibles es del tamaño necesario como para escribirse por salida estandar.
    if (count >= minlength) escribir(count, bufout);

    //liberar memoria
    free(bufin);
    free(bufout);

    return EXIT_SUCCESS;
}

        