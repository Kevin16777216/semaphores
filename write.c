#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define KEY 24602
#define SIZE 200
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};
const char* file = "m.txt";
int main() {
    int sem = semget(KEY, 1, 0);
    int shm = shmget(KEY, 1, 0);
    struct sembuf s;
    s.sem_flg = SEM_UNDO;
    s.sem_op = -1;
    s.sem_num = 0;
    printf("trying to get in\n");
    if(sem == -1) {
        printf("The sempahore does not exist. Please run ./control -c to create it.\n");
    }
    if(shm == -1) {
        printf("The shared memory does not exist. Please run ./control -c to create it.\n");
        s.sem_op = 1;
        semop(sem, &s, 1);
    }
    char *m = shmat(shm,0,0);
    printf("Last addition: %s\nYour addition: ", m);
    char input[SIZE];
    fgets(m, SIZE, stdin);
    strcpy(input, m);
    int fd = open(file, O_RDWR | O_APPEND, 644);
    write(fd,input,strlen(input));
    close(fd);
    s.sem_op = 1;
    semop(sem, &s, 1);
    shmdt(m);
}
