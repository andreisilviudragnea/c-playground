#include "common.h"
#include "util.h"

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

static volatile sig_atomic_t loop = 1;

static void signal_handler(int signal unused)
{
    loop = 0;
}

/**
 * Read buffers of data data from standard input into a POSIX shared memory
 * object from which it is copied by test_reader.c
 *
 * We use a pair of binary semaphores to ensure that the writer and reader have
 * exclusive, alternating access to the shared memory. (i.e., the writer writes
 * a block of text, then the reader reads, then the writer writes etc). This
 * ensures that each block of data is processed in turn by the writer and
 * reader.
 *
 * This program needs to be started before the reader process as it creates the
 * shared memory and semaphores used by both processes.
 *
 * Together, these two programs can be used to transfer a stream of data through
 * shared memory as follows:
 *
 * $ test_writer < infile &
 * $ test_reader > out_file
 */
int main(int argc unused, char *argv[] unused)
{
    DIE(signal(SIGINT, signal_handler) == SIG_ERR, "signal SIGINT");
    DIE(signal(SIGTERM, signal_handler) == SIG_ERR, "signal SIGTERM");

    sem_t *write_sem = sem_open(WRITE_SEM_NAME, O_CREAT | O_RDWR, 0600, 1);
    DIE(write_sem == SEM_FAILED, "sem_open write_sem");

    sem_t *read_sem = sem_open(READ_SEM_NAME, O_CREAT | O_RDWR, 0600, 0);
    DIE(read_sem == SEM_FAILED, "sem_open read_sem");

    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
    DIE(fd == -1, "shm_open");

    DIE(ftruncate(fd, BUF_SIZE) == -1, "ftruncate");

    struct shmseg *shmp = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE,
                               MAP_SHARED, fd, 0);
    DIE(shmp == MAP_FAILED, "mmap");

    DIE(close(fd) == -1, "close");

    while (loop) {
        DIE(sem_wait(write_sem) == -1, "sem_wait write_sem");

        ssize_t num_read = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
        DIE(num_read == -1, "read");
        shmp->cnt = (size_t) num_read;

        DIE(sem_post(read_sem) == -1, "sem_post read_sem");

        /* Have we reached EOF? We test this after giving the reader
           a turn so that it can see the 0 value in shmp->cnt. */
        if (shmp->cnt == 0) {
            break;
        }
    }

    /* Wait until reader has let us have one more turn. We then know
       reader has finished, and so we can delete the IPC objects. */
    DIE(sem_wait(write_sem) == -1, "sem_wait write_sem");

    DIE(sem_close(read_sem) == -1, "sem_close read_sem");
    DIE(sem_unlink(READ_SEM_NAME) == -1, "sem_unlink read_sem");

    DIE(sem_close(write_sem) == -1, "sem_close write_sem");
    DIE(sem_unlink(WRITE_SEM_NAME) == -1, "sem_unlink write_sem");

    DIE(shm_unlink(SHM_NAME) == -1, "shm_unlink");

    return 0;
}
