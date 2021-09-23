#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char *argv[])
{
    pid_t pid;
    int fd, status;
    char* file_name;

    if (argc < 2){
        fprintf(stderr, "USO: openpdf FILE.pdf\n");
        return EXIT_FAILURE;
    }

    if(access(argv[1], F_OK | R_OK) == 1) {
        perror("access()");
        exit(EXIT_FAILURE);
    }

    file_name = argv[1];

    switch (pid=fork())
    {
    case -1:    //error
        perror("fork()");
        exit(EXIT_FAILURE);
        break;

    case 0:
        execlp("evince", "evince", file_name, NULL);
        break;

    default:
        if (waitpid(pid, &status, 0) == -1) /* Espera a que termine el proceso hijo */
            {
                perror("wait()");
                exit(EXIT_FAILURE);
            }
        break;
    }



    return EXIT_SUCCESS;
}
