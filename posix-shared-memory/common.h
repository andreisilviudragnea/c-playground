#ifndef PROJECT_COMMON_H
#define PROJECT_COMMON_H

#include <sys/types.h>

#define SHM_NAME "/demo_shm"
#define READ_SEM_NAME "/demo_read_sem"
#define WRITE_SEM_NAME "/demo_write_sem"

#ifndef BUF_SIZE                /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024           /* Size of transfer buffer */
#endif

struct shmseg {                 /* Defines structure of shared memory segment */
    size_t cnt;                 /* Number of bytes used in 'buf' */
    char buf[BUF_SIZE];         /* Data being transferred */
};

#endif //PROJECT_COMMON_H
