#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 4096

/*  Ejemplo: cat_mem_din [-o FILEOUT] [FILEIN1 FILEIN2 ... FILEINn]
    Notas:   Por defecto, se escribe en 'stdout' y se lee de 'stdin'.
*/

/*ssize_t writeall(int fdout, char *buf, size_t count){
    ssize_t num_written, num_left;
    char *buf_left;
    num_left = count;
    buf_left = buf;

    while (num_left > 0 && ((num_written = write(fdout, buf, ))
    return NULL;
}*/


void catfd(int fdin, int fdout, char *buf, unsigned buf_size)
{
    ssize_t num_read, num_written, num_written_tot, num_left;
    char *buf_left;

    while ((num_read = read(fdin, buf, buf_size)) > 0)
    {
        num_left = num_read;
        buf_left = buf;
        // ESCRITURAS PARCIALES -> en caso de que se escriba menos de lo que el buffer tiene,llevamos un contador de qué lleva escrito y se vuelve a escribir lo que queda.
        while((num_left) > 0 && ((num_written = write(fdout, buf_left, num_left)) > 0))
        {                                                                                               
            //num_written_tot += num_written;
            num_left = num_read-num_written;
            buf_left += num_written;
        }
        if (num_written == -1)
        {
            perror("write(fdin)");
            exit(EXIT_FAILURE);
        }
        /* Escrituras parciales no tratadas */
        assert(num_written == num_read);
    }

    if (num_read == -1)
    {
        perror("read(fdin)");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    int opt;
    char *fileout = NULL;
    int fdout;
    int fdin;
    char *buf;

    optind = 1;
    while ((opt = getopt(argc, argv, "o:h")) != -1)
    {
        switch (opt)
        {
        case 'o':
            fileout = optarg;
            break;
        case 'h':
        default:
            fprintf(stderr, "Uso: %s [-o FILEOUT] [FILEIN1 FILEIN2 ... FILEINn]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    /* Abre el fichero de salida */
    if (fileout != NULL)
    {
        fdout = open(fileout, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (fdout == -1)
        {
            perror("open(fileout)");
            exit(EXIT_FAILURE);
        }
    }
    else /* Por defecto, la salida estándar */
        fdout = STDOUT_FILENO;

    /* Reserva memoria dinámica para buffer de lectura */
    if ((buf = (char *) malloc(BUF_SIZE * sizeof(char))) == NULL)
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }
/*
    int fderror;
    if (fderror = open("error.log", O_WRONLY | O_CREAT |  O_TRUNC, S_IRUSR | S_IWUSR) == -1)
    {
        perror("open(fderror)");
        exit(EXIT_FAILURE);
    }

    if(dup2(fderror, STDERR_FILENO) == -1){
        perror("dup2()");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Salida de error 1\n");
    write(STDERR_FILENO, "Salida de error 2\n", strlen("Salida de error\n"));*/

    /* Abre cada fichero de entrada y lo escribe en 'fileout' */
    if (optind < argc)
        for (int i = optind; i < argc; i++)
        {
            int fdin = open(argv[i], O_RDONLY);
            if (fdin == -1)
            {
                perror("open(filein)");
                continue;
            }
            catfd(fdin, fdout, buf, BUF_SIZE);
            if (close(fdin) == -1)
            {
                perror("close(fdin)");
                exit(EXIT_FAILURE);
            }
        }
    else
    {
        catfd(STDIN_FILENO, fdout, buf, BUF_SIZE);
    }

    if (close(fdout) == -1)
    {
        perror("close(fdout)");
        exit(EXIT_FAILURE);
    }

    /* Libera memoria dinámica de buffer de lectura */
    free(buf);

    exit(EXIT_SUCCESS);
}
