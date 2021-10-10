#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#define DEFAULT_BUFSIZE 1024    // tamaño del buffer por defecto


int fin_all(int entradas, int *terminado)   //devuelve verdadero cuando todos los ficheros de lectura han sido leidos
{
    for (int i = 0; i < entradas; i++)
        if (terminado[i] == 0) return 0;
    return 1;
}

int escribiendo(int entradas, ssize_t *numleft) // devuelve verdadero mientras quede contenido en algún buffer de lectura
{
    for (int i = 0; i < entradas; i++)
        if (numleft[i] > 0) return 1;
    return 0;
}

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
    
    /* Reserva memoria dinámica para buffer de salida */
    if ((bufout = (char *) malloc(bufsize * sizeof(char))) == NULL)
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }
    //se crean tantos descriptores como ficheros de entrada haya
    int *descriptores;
    if ((descriptores = malloc(entradas * sizeof(int))) == NULL)
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    // reservar memoria para que haya un buffer de lectura por cada fichero de entrada abierto
    if ((bufin = malloc(entradas * sizeof(char*))) == NULL)
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }


    //abrir los ficheros de entrada e inicializar los bufferes correspondientes a cada fichero de entrada
    for (int i = 0; i < entradas; i++)
    {
        descriptores[i] = open(argv[i+optind], O_RDONLY);
        if (descriptores[i]==-1)
        {
            fprintf(stderr, "Error: No se puede abrir el fichero '%s': ", argv[i+optind]);
            perror("");
            // exit(EXIT_FAILURE);      - no salimos de la ejecución, simplemente ignoramos este fichero
            optind++;   //aumentamos para pasar al siguiente elemento de los argumentos, ignorando al erroneo
            i--;        //disminuimos la i para repetir la iteración
            entradas--; //disminuimos el numero de entradas que tiene el problema en 1, puesto que una de ellas era erronea.

        }

        bufin[i] = malloc(bufsize * sizeof(char));  // reservar la memoria para el buffer de lectura del fichero i
    }

    // El hecho de ignorar los ficheros de entrada que no existen, implica que se pueda dar la situación de que todos los ficheros 
    // de entrada puedan ser erróneos, en cuyo caso se debe abortar la ejecución. Se debe tratar esta situación.
    if (entradas == 0)
    {
        fprintf(stderr, "Error: Ningún fichero de entrada ha podido ser leído. Abortando ejecución.\n");
        exit(EXIT_FAILURE);
    }
    
    int *terminado = calloc(entradas, sizeof(int));
    ssize_t *numleft = calloc(entradas, sizeof(ssize_t));
    ssize_t *numwritten = calloc(entradas, sizeof(ssize_t));

    //al lio
    while (!fin_all(entradas,terminado)) //sale del bucle cuando todos los ficheros están completamente leídos
    {
        for (int i = 0; i < entradas; i++)
        {
            if(!terminado[i])   //se revisa si el fichero está ya leído al completo
            {
                numleft[i] = read(descriptores[i], bufin[i], bufsize);  //guardamos el número de bytes leídos de este fichero, que serán los que habrá que escribir exactamente en el bufer de escritura (para tratar posibles escrituras parciales)
                 if (numleft[i]==-1)
                {
                    perror("read(fdin)");
                    exit(EXIT_FAILURE);
                }
                if (numleft[i]==0)  // tratar los casos en los que el fichero acaba, cerrando el desriptor y marcando que el fichero ya no debe volver a leerse.
                {
                    terminado[i] = 1;
                    if (close(descriptores[i]) == -1)
                    {
                        perror("close(fdin)");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
        int count = 0;  // variable auxiliar que mantiene la posición por la que estamos escribiendo en el buffer de escritura
        //ahora las escrituras: se hacen las mezclas en el buffer de salida
        while(escribiendo(entradas, numleft))   //no sale de este bucle mientras quede contenido en alguno de los buffers de los ficheros
        {    
            for (int i = 0; i < entradas; i++)  //para cada buffer de lectura
            {
                if (numleft[i] > 0)     // si queda algo que leer
                {
                    bufout[count] = bufin[i][numwritten[i]];
                    numwritten[i]++;
                    numleft[i]--;
                    count++;
                    if (count == bufsize)   // se ha llenado el buffer antes de terminar de leer todos los ficheros
                    {
                        char *buf_left;
                        buf_left = bufout;
                        int num_written_out;
                        // ESCRITURAS PARCIALES -> en caso de que se escriba menos de lo que el buffer tiene,llevamos un contador de qué lleva escrito y se vuelve a escribir lo que queda.
                        while((count) > 0 && ((num_written_out = write(fdout, buf_left, count)) > 0))
                        {                                                                                               
                            count -= num_written_out;
                            buf_left += num_written_out;
                        }
                        if (num_written_out == -1)
                        {
                            perror("write(fdin)");
                            exit(EXIT_FAILURE);
                        }
                        assert(num_written_out == bufsize);
                    }
                }
            }
        }
        if (count > 0)   // se ha terminado la lectura de los buffers de entrada, y no ha llenado el buffer de escritura
        {
            char *buf_left;
            buf_left = bufout;
            int num_written_out;
            // ESCRITURAS PARCIALES -> en caso de que se escriba menos de lo que el buffer tiene,llevamos un contador de qué lleva escrito y se vuelve a escribir lo que queda.
            while((count) > 0 && ((num_written_out = write(fdout, buf_left, count)) > 0))
            {                                                                                               
                count -= num_written_out;
                buf_left += num_written_out;
            }
            if (num_written_out == -1)
            {
                perror("write(fdin)");
                exit(EXIT_FAILURE);
            }
        }
        for (int i = 0; i < entradas; i++) numwritten[i] = 0; //reseteamos el numero de carácteres escritos de cada buffer, ya que en la siguiente iteración comienza una lectura/ escritura nueva
    }
       
    //liberar memoria
    free(numwritten);
    free(numleft);
    free(terminado);
    for (int i = 0; i < entradas; i++) free(bufin[i]);  // primero libera todos los bufferes correspondientes a ficheros de entrada
    free(bufin);    //despues libera el array de bufferes
    free(descriptores); //libera la memoria del array de descriptores
    free(bufout);   //libera la memoria del buffer de salida

    return EXIT_SUCCESS;
}

