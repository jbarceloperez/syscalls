#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#define DEFAULT_BUFSIZE 1024    // tamaño del buffer por defecto


// void merge(int bufsize, int entradas, char *argv){

//     char *bufout;
//     char **bufin;

//     // Reserva memoria dinámica para buffer de salida
//     if ((bufout = (char *) malloc(bufsize * sizeof(char))) == NULL)
//     {
//         perror("malloc()");
//         exit(EXIT_FAILURE);
//     }
//     //se crean tantos descriptores como ficheros de entrada haya
//     int *descriptores = malloc(entradas * sizeof(int));

//     // reservar memoria para que haya un buffer de lectura por cada fichero de entrada abierto
//     bufin = malloc(entradas * sizeof(char*));


//     //abrir los ficheros de entrada e inicializar los bufferes correspondientes a cada fichero de entrada
//     for (int i = 0; i < entradas; i++)
//     {
//         descriptores[i] = open(argv[i+optind], O_RDONLY);
//         if (descriptores[i]==-1)
//         {
//             perror("open(filein)");
//             exit(EXIT_FAILURE);
//         }

//         bufin[i] = malloc(bufsize * sizeof(char));  // reservar la memoria para el buffer de lectura del fichero i
//     }
    
    

    
//     for (int i = 0; i < entradas; i++) free(bufin[i]);  // primero libera todos los bufferes correspondientes a ficheros de entrada
//     free(bufin);    //despues libera el array de bufferes
//     free(descriptores); //libera la memoria del array de descriptores
//     free(bufout);   //libera la memoria del buffer de salida

// }



int main(int argc, char *argv[])
{
    int opt, bufsize = DEFAULT_BUFSIZE, fdout;
    char *fileout = NULL;
    char *bufout;
    char **bufin;

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

    int entradas = argc - optind;   // número de ficheros de entrada 
    // si hay más de 16 ficheros de entrada
    if ((entradas)>16)
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
    
    // EJECUCIÓN - TENEMOS AL MENOS UN FICHERO DEL CUAL LEER
    
    //merge(bufsize, entradas, argv);


    /* Reserva memoria dinámica para buffer de salida */
    if ((bufout = (char *) malloc(bufsize * sizeof(char))) == NULL)
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }
    //se crean tantos descriptores como ficheros de entrada haya
    int *descriptores = malloc(entradas * sizeof(int));

    // reservar memoria para que haya un buffer de lectura por cada fichero de entrada abierto
    bufin = malloc(entradas * sizeof(char*));


    //abrir los ficheros de entrada e inicializar los bufferes correspondientes a cada fichero de entrada
    for (int i = 0; i < entradas; i++)
    {
        descriptores[i] = open(argv[i+optind], O_RDONLY);
        if (descriptores[i]==-1)
        {
            perror("open(filein)");
            exit(EXIT_FAILURE);
        }

        bufin[i] = malloc(bufsize * sizeof(char));  // reservar la memoria para el buffer de lectura del fichero i
    }
    
    int *escribiendo = calloc(entradas, sizeof(int));
    ssize_t *numread = calloc(entradas, sizeof(ssize_t));
    ssize_t *numwritten = calloc(entradas, sizeof(ssize_t));
    //al lio
    while (1)
    {
        for (int i = 0; i < entradas; i++)  //primero se hace una lectura de tamaño bufsize de cada fichero si es necesario
        {
            if(!escribiendo[i]) // si escribiendo es 1 es que aun queda información en el buffer de la lectura anterior
            {   //se almacena el no. de bytes leidos para cada fichero
                numread[i] = read(descriptores[i], bufin[i], bufsize);
                escribiendo[i] = 1;
                if (numread[i]==-1)
                {
                    perror("read(fdin)");
                    exit(EXIT_FAILURE);
                }
            }
        }

        for (int i = 0; i < entradas; i++)  //despues escribimos un byte de cada fichero en la salida
        {

        }
    }
    

    
    //liberar memoria
    free(numread);
    free(escribiendo);
    for (int i = 0; i < entradas; i++) free(bufin[i]);  // primero libera todos los bufferes correspondientes a ficheros de entrada
    free(bufin);    //despues libera el array de bufferes
    free(descriptores); //libera la memoria del array de descriptores
    free(bufout);   //libera la memoria del buffer de salida

    return EXIT_SUCCESS;
}

