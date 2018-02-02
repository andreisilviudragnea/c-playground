#include "common.h"
#include "util.h"

#include <stdio.h>
#include <signal.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <semaphore.h>

static volatile sig_atomic_t loop = 1;

static void signal_handler(int signal unused)
{
    loop = 0;
}

int main(int argc unused, char *argv[] unused)
{
    DIE(signal(SIGINT, signal_handler) == SIG_ERR, "signal SIGINT");
    DIE(signal(SIGTERM, signal_handler) == SIG_ERR, "signal SIGTERM");

    sem_t *write_sem = sem_open(WRITE_SEM_NAME, O_RDWR, 0600);
    DIE(write_sem == SEM_FAILED, "sem_open write_sem");

    sem_t *read_sem = sem_open(READ_SEM_NAME, O_RDWR, 0600);
    DIE(read_sem == SEM_FAILED, "sem_open read_sem");

    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
    DIE(fd == -1, "shm_open");

    struct shmseg *shmp = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE,
                               MAP_SHARED, fd, 0);
    DIE(shmp == MAP_FAILED, "mmap");

    DIE(close(fd) == -1, "close");

    while (loop) {
        /* Wait for our turn */
        DIE(sem_wait(read_sem) == -1, "sem_wait read_sem");

        /* Writer encountered EOF */
        if (shmp->cnt == 0)
            break;

        ssize_t num_written = write(STDOUT_FILENO, shmp->buf, shmp->cnt);
        DIE(num_written == -1 || (size_t) num_written != shmp->cnt, "write");

        /* Give writer a turn */
        DIE(sem_post(write_sem) == -1, "sem_post write_sem");
    }

    /* Give writer one more turn, so it can clean up */
    DIE(sem_post(write_sem) == -1, "sem_post write_sem");

    DIE(sem_close(read_sem) == -1, "sem_close read_sem");

    DIE(sem_close(write_sem) == -1, "sem_close write_sem");

    return EXIT_SUCCESS;
}
