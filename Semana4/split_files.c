#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>                          // Javier Barceló Pérez, Grupo 3.1 - Tarea Semana 4 ASO
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#define DEFAULT_BUFSIZE 1024


void escribir(ssize_t *count, char *buf, int fd)
{
    size_t num_left, num_written;
    char *buf_left;

    num_left = *count;
    buf_left = buf;
    //escrituras
    while((num_left) > 0 && ((num_written = write(fd, buf_left, num_left)) > 0))
    {
        num_left -= num_written;
        buf_left += num_written;
    }
    if (num_written == -1)
    {
        perror("write(fd)");
        exit(EXIT_FAILURE);
    }
    *count = 0;
}

int main(int argc, char *argv[])
{
    int opt, bufsize = DEFAULT_BUFSIZE;
    int *descriptores;  //aqui se almacenaran los descriptores de los ficheros de salida
    char *bufin;
    char **bufout; 

    int fd = STDIN_FILENO;  //se lee de la entrada estandar
    
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
        fprintf(stderr, "Uso: %s [-t BUFSIZE] FILEOUT1 [FILEOUT2 ... FILEOUTn]\nNDivide en ficheros el flujo de bytes que recibe por la entrada estandar.\nEl numero maximo de ficheros de salida es 16.\n -t BUFSIZE  Tamaño de buffer donde 1 <= BUFSIZE <= 1MB (por defecto 1024).\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int ficheros = argc - optind;

    // en caso de que haya más de 16 ficheros de salida
    if (ficheros > 16)
    {
        fprintf(stderr, "Error: Demasiados ficheros de salida.\n");
        fprintf(stderr, "Uso: %s [-t BUFSIZE] FILEOUT1 [FILEOUT2 ... FILEOUTn]\nNDivide en ficheros el flujo de bytes que recibe por la entrada estandar.\nEl numero maximo de ficheros de salida es 16.\n -t BUFSIZE  Tamaño de buffer donde 1 <= BUFSIZE <= 1MB (por defecto 1024).\n", argv[0]);
        exit(EXIT_FAILURE); 
    }

    // comprobar el tamaño del buffer
    if (bufsize < 1 || bufsize > 1048576)
    {
        fprintf(stderr, "Error: Tamaño de buffer incorrecto.\n\n");
        fprintf(stderr, "Uso: %s [-t BUFSIZE] FILEOUT1 [FILEOUT2 ... FILEOUTn]\nNDivide en ficheros el flujo de bytes que recibe por la entrada estandar.\nEl numero maximo de ficheros de salida es 16.\n -t BUFSIZE  Tamaño de buffer donde 1 <= BUFSIZE <= 1MB (por defecto 1024).\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //ejecucion

    //reservar memoria para el buffer de lectura
    if ((bufin = (char *) malloc(bufsize * sizeof(char))) == NULL)
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    //se crean tantos descriptores como ficheros de entrada haya
    if ((descriptores = malloc(ficheros * sizeof(int))) == NULL)
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    // reservar memoria para que haya un buffer de escritura por cada fichero de salida abierto
    if ((bufout = malloc(ficheros * sizeof(char*))) == NULL)
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    //abrir los ficheros de salida e inicializar los bufferes correspondientes a cada fichero de entrada
    for (int i = 0; i < ficheros; i++)
    {
        descriptores[i] = open(argv[i+optind], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
        if (descriptores[i]==-1)
        {
            fprintf(stderr, "Error: No se puede abrir/crear el fichero '%s': ", argv[i+optind]);
            perror("");
            exit(EXIT_FAILURE);
        }
        if ((bufout[i] = (char *) malloc(bufsize * sizeof(char))) == NULL)   // reservar la memoria para el buffer de escritura del fichero i
        {
            perror("malloc()");
            exit(EXIT_FAILURE);
        }
    }

    ssize_t *buf_offset;
    if ((buf_offset = calloc(ficheros, sizeof(ssize_t))) == NULL)
    {
        perror("calloc()");
        exit(EXIT_FAILURE);        
    }

    ssize_t num_read, num_left;
    int index = 0;
    // fd = open("in", O_RDONLY);//DEBUG

    // mientras queden cosas por leer de la entrada estandar, se ejecuta el bucle
    while ((num_read = read(fd, bufin, bufsize)) > 0)
    {
        num_left = num_read;
        while (num_left > 0)       //mientras queden bytes por leer del buffer de lectura
        {
            bufout[index][buf_offset[index]] = bufin[num_read - num_left];  //se escribe en la siguiente posición del buffer de escritura index el byte correspondiente del buffer de lectura
            buf_offset[index]++;    //se suma uno al tamaño del buffer index
            num_left--; // disminuye el numero de bytes restantes del buffer de lectura
            if (buf_offset[index] == bufsize) // si se llena el buffer de escritura de un fichero
            {
                escribir(&buf_offset[index], bufout[index], descriptores[index]);   //se escribe en el fichero correspondiente
            }
            index++;
            index = index % ficheros;   // en cada iteración index toma el valor de uno de los ficheros, y al llegar al ultimo vuelve a empezar
        }
    }

    //comprobar si queda algún buffer con contenido que no se haya imprimido
    for (int i = 0; i < ficheros; i++)
    {
        if (buf_offset[i] != 0)
        {
            escribir(&buf_offset[i], bufout[i], descriptores[i]);
        }
    }



    //liberar bufferes
    free(buf_offset);
    for (int i = 0; i < ficheros; i++) free(bufout[i]);
    free(bufout);
    free(descriptores);
    free(bufin);

    return EXIT_SUCCESS;
}
