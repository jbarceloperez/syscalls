#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char **argv)
{
    pid_t pid; /* Usado en el proceso padre para guardar el PID del proceso hijo */
    int fd, status;
    char *mis_args[3];

    switch (pid = fork())
    {
    case -1: /* fork() falló */
        perror("fork()");
        exit(EXIT_FAILURE);
        break;
    case 0:                             /* Ejecución del proceso hijo tras fork() con éxito */
        if (close(STDOUT_FILENO) == -1) /* Cierra la salida estándar */
        {
            perror("close()");
            exit(EXIT_FAILURE);
        }
        /* Abre el fichero "listado" al que se asigna el descriptor de fichero no usado más bajo, es decir, STDOUT_FILENO(1) */
        if ((fd = open("listado", O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) == -1)
        {
            perror("open()");
            exit(EXIT_FAILURE);
        }
        //inicializamos valores de los args del ls
        mis_args[0] = "sleep";
        mis_args[1] = "-la";
        mis_args[2] = NULL;
        execvp("sleep", mis_args);      /* Sustituye el binario actual por /bin/ls */ //si al funcion exec incluye un p, en el binario podemos poner el nombre del binario en vez de la ruta si está en el PATH
        perror("exec()");   //mejor esto que lo de abajo
//      fprintf(stderr, "execlp() failed\n"); /* Esta línea no se debería ejecutar si la anterior tuvo éxito */
        exit(EXIT_FAILURE);
        break;
    default:                  /* Ejecución del proceso padre tras fork() con éxito */
        kill(pid, SIGKILL);
        if (waitpid(pid, &status, 0) == -1) /* Espera a que termine el proceso hijo */
        {
            perror("wait()");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status))
            printf("PID %d ha terminado bien con salida %d", pid, WEXITSTATUS(status));

        if (WIFSIGNALED(status))
            printf("PID %d ha terminado mal con salida %d", pid, WTERMSIG(status));
        break;
    }

    return EXIT_SUCCESS;
}
