#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>                          // Javier Barceló Pérez, Grupo 3.1 - Tarea Semana 4 ASO
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>     // para el strtok_r()
#include <sys/wait.h>


#define DEFAULT_BUFSIZE 1024    // tamaño del buffer por defecto
#define DEFAULT_MINLENGTH 4     // longitud minima por defecto para el mystrings


void print_help()
{
    fprintf(stderr, "Uso: ./merge_mystrings_split [-t BUFSIZE] [-n MIN_LENGTH] -i FILEIN1[,FILEIN2,...,FILEINn] FILEOUT1 [FILEOUT2 ...]\n  -t BUFSIZE     Tamaño de buffer donde 1 <= BUFSIZE <= 1MB (por defecto 1024).\n  -n MINLENGTH   Longitud mínima de la cadena. Mayor que 0 y menor que 256 (por defecto 4).\n  -i             Lista de ficheros de entrada separados por comas.\n");
}

int main(int argc, char *argv[])
{
    int opt, bufsize = DEFAULT_BUFSIZE, minlength = DEFAULT_MINLENGTH;
    char *inputs = NULL;
    char *entrada[16];
    char **salida;

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
            print_help();
            exit(EXIT_FAILURE);
        }
    }

    // comprobar el tamaño del buffer
    if (bufsize < 1 || bufsize > 1048576)
    {
        fprintf(stderr, "Error: Tamaño de buffer incorrecto.\n\n");
        print_help();
        exit(EXIT_FAILURE);
    }

    //comprobar la longitud minima
    if (minlength < 1 || minlength > 255)
    {
        fprintf(stderr, "ERROR: La longitud mínima debe estar entre 1 y 255\n");
        print_help();
        exit(EXIT_FAILURE);
    }

    //comprueba que se han introducido ficheros de entrada
    if (inputs == NULL)
    {
        fprintf(stderr, "Error: Se deben proporcionar ficheros de entrada separados por comas.\n");
        print_help();
        exit(EXIT_FAILURE);
    }

    // guarda los ficheros de entrada en la estructura char *entrada[] y el número de ellos en count
    char *ptrToken; 
    char *saveptr; 
    int count = 0;
    ptrToken = strtok_r(inputs, ",", &saveptr); 
    while (ptrToken != NULL) 
    {   
        entrada[count] = ptrToken;
        count++;
        ptrToken = strtok_r(NULL, ",", &saveptr);
        if (count > 15 && ptrToken == NULL) //no puede haber más de 16 ficheros de entrada
        {
            fprintf(stderr, "Error: Demasiados ficheros de entrada. Máximo 16.\n");
            print_help();
            exit(EXIT_FAILURE);
        }
    }

    //comprueba los ficheros de salida
    if (argc == optind)
    {
        fprintf(stderr, "Error: Deben proporcionarse ficheros de salida.\n");
        print_help();
        exit(EXIT_FAILURE);
    }

    // ejecución

    //crea las tuberías
    int pip1[2];
    if (pipe(pip1) == -1) 
    {
        perror("pipe()");
        exit(EXIT_FAILURE);
    }
    int pip2[2];
    if (pipe(pip2) == -1) 
    {
        perror("pipe()");
        exit(EXIT_FAILURE);
    }

    // creamos el hijo izquierdo de la primera tubería
    char *args_merge[4 + count];
    printf("SOY EL PADRE EJECUTANDO EL PRIMER FORK\n");    //DEBUG
    switch (fork())
    {
    case -1:    //error en el fork
        perror("fork(1)");
        exit(EXIT_FAILURE);
        break;
    case 0:     // es el hijo
        if (close(pip1[0]) == -1)   //se cierran los descriptores que no se usan
        {
            perror("close(pip1[0])");
            exit(EXIT_FAILURE);
        }
        if (close(pip2[0]) == -1)
        {
            perror("close(pip2[0])");
            exit(EXIT_FAILURE);
        }
        if (close(pip2[1]) == -1)
        {
            perror("close(pip2[1])");
            exit(EXIT_FAILURE);
        }
        if (dup2(pip1[1], STDOUT_FILENO) == -1) // duplicar el extremo de lectura de la tubería a la salida estándar
        {
            perror("dup2(1)");
            exit(EXIT_FAILURE);
        }
        if (close(pip1[1]) == -1)    //cerrar el descriptor de la tubería
        {
            perror("close(pip1[1])");
            exit(EXIT_FAILURE);
        }
        // preparamos argumentos y hacemos el exec
        args_merge[0] = "merge_files";
        args_merge[1] = "-t";
        sprintf(args_merge[2],"%d",bufsize);
        // args_merge[2] = "1024"; //DEBUG 
        for (int i = 0; i < count; i++)
            args_merge[i+3] = entrada[i];
        args_merge[count+3] = NULL;
        execv("merge_files", args_merge);
        perror("execv(merge_files)");   // si no se ha producido el execv entonces hay que abortar la ejecución
        exit(EXIT_FAILURE);
        break;
    default:    // es el padre, continúa la ejecución
        break;
    }

    // creamos el hijo del medio, el derecho de la primera tubería e izquierdo de la segunda (mystrings)
    char *args_mystrings[6];
    printf("SOY EL PADRE EJECUTANDO EL SEGUNDO FORK\n");    //DEBUG
    switch (fork())
    {
    case -1:
        perror("fork(2)");
        exit(EXIT_FAILURE);
        break;
    case 0: // es el hijo
        if (close(pip1[1]) == -1)   //se cierran los descriptores que no se usan
        {
            perror("close(pip1[1])");
            exit(EXIT_FAILURE);
        }
        if (close(pip2[0]) == -1)
        {
            perror("close(pip2[0])");
            exit(EXIT_FAILURE);
        }
        // redirigir la entrada estandar al extremo izquierdo de la primera tubería
        if (dup2(pip1[0], STDIN_FILENO) == -1)
        {
            perror("dup2(2)");
            exit(EXIT_FAILURE);
        }
        // // redirigir la salida estandar al extremo derecho de la segunda tubería
        // if (dup2(pip2[1], STDOUT_FILENO) == -1)
        // {
        //     perror("dup2(3)");
        //     exit(EXIT_FAILURE);
        // }
        if (close(pip1[0]) == -1)   //se cierran los descriptores duplicados
        {
            perror("close(pip1[0])");
            exit(EXIT_FAILURE);
        }
        if (close(pip2[1]) == -1)
        {
            perror("close(pip2[1])");
            exit(EXIT_FAILURE);
        }
        //preparamos los argumentos y hacemos el execv
        args_mystrings[0] = "mystrings";
        args_mystrings[1] = "-t";
        sprintf(args_mystrings[2],"%d",bufsize);
        args_mystrings[3] = "-n";
        sprintf(args_mystrings[4],"%d",minlength);
        args_mystrings[5] = NULL;
        fprintf(stdout,"[0]%s[1]%s[2]%s[3]%s[4]%s\n\n",args_mystrings[0],args_mystrings[1],args_mystrings[2],args_mystrings[3],args_mystrings[4]);
        execv("mystrings", args_mystrings);
        perror("execv(mystrings)");
        exit(EXIT_FAILURE);
        break;
    default:    // es el padre, prosigue la ejecución
        break;
    }

    // creamos el ultimo hijo, el de la derecha de la segunda tubería (split_files)
    int ficheros = argc - optind;
    char *args_split[4+ficheros];
    switch (fork())
    {
    case -1:
        perror("fork(3)");
        exit(EXIT_FAILURE);
        break;
    case 0: // es el hijo
        if (close(pip1[0]) == -1)   //se cierran los descriptores que no se usan
        {
            perror("close(pip1[0])");
            exit(EXIT_FAILURE);
        }
        if (close(pip1[1]) == -1)
        {
            perror("close(pip1[1])");
            exit(EXIT_FAILURE);
        }
        if (close(pip2[1]) == -1)
        {
            perror("close(pip2[1])");
            exit(EXIT_FAILURE);
        }
        // redirigir la entrada estandar al extremo izquierdo de la segunda tubería
        if (dup2(pip2[0], STDIN_FILENO) == -1)
        {
            perror("dup2(4)");
            exit(EXIT_FAILURE);
        }
        if (close(pip2[0]) == -1)    //cerrar el descriptor de la tubería
        {
            perror("close(pip2[0])");
            exit(EXIT_FAILURE);
        }
        //preparamos los args y execv
        args_split[0] = "split_files";
        args_split[1] = "-t";
        sprintf(args_split[2],"%d",bufsize);
        for (int i = 0; i < ficheros; i++)
            args_split[3+i] = argv[optind+i];
        args_split[ficheros+3] = NULL;
        execv("split_files", args_split);
        perror("execv(split_files)");
        exit(EXIT_FAILURE);
        break;
    default:        //el padre, seguimos
        break;
    }

    // siendo el padre, cerramos los descriptores de las tuberias
    if (close(pip1[0]) == -1)
    {
        perror("close(pip1[0])");
        exit(EXIT_FAILURE);
    }
    if (close(pip1[1]) == -1)
    {
        perror("close(pip1s[1])");
        exit(EXIT_FAILURE);
    }
    if (close(pip2[0]) == -1)
    {
        perror("close(pip2[0])");
        exit(EXIT_FAILURE);
    }
    if (close(pip2[1]) == -1)
    {
        perror("close(pip2[1])");
        exit(EXIT_FAILURE);
    }

    // esperar a que los tres hijos terminen sin ningún orden concreto
    if (wait(NULL) == -1)
    {
        perror("wait(1)");
        exit(EXIT_FAILURE);
    }
    if (wait(NULL) == -1)
    {
        perror("wait(2)");
        exit(EXIT_FAILURE);
    }
    if (wait(NULL) == -1)
    {
        perror("wait(3)");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
