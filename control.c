#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define KEY 24602
#define SIZE 255
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};
void rd(){
  FILE* file = fopen("m.txt", "r");
  char line[SIZE];
  while(fgets(line, SIZE, file))
      printf("%s", line);
}
char ** parse_args( char * line ) {
    char *token;
    char **out = malloc(strlen(line) * sizeof(char *));
    int i;
    for(i = 0; line; ++i) {
        char * tmp = strsep(&line," ");
        out[i] = tmp;
    }
    return out;
}
int main(int argc, char * argv[]) {
    FILE *file;
    char* data;
    char input[256] = "";
    int j;
    if (argc > 1) {
        for(j = 1; j < argc; ++j) {
            if(strlen(argv[j]) > 1) {
                strcat(input, argv[j]);
                strcat(input, " ");
            }
        }
    } else {
        printf("Please enter an argument:");
        fgets(input, 254, stdin);
    }
    input[strlen(input)-1] = '\0';
    char ** in = parse_args(input);
    if(!in[0]) {
        printf("Error : need to have input\n");
        exit(0);
    }
    int sem,shm;
    if(!strcmp(in[0],"-c")) {
        int r,v;
        char* data;
        sem = semget(KEY, 1, IPC_CREAT | IPC_EXCL | 0644);
        if (sem == -1) {
            sem = semget(KEY, 1, 0);
            v = semctl(sem, 0, GETVAL, 0);
            printf("semaphore already exists\n");
        } else {
            union semun us;
            us.val = 1;
            r = semctl(sem, 0, SETVAL, us);
            printf("semaphore created\n");
        }
        shm = shmget(KEY, SIZE, IPC_CREAT | 0644);
        printf("shared memory created\n");
        data = shmat(shm, 0, 0);
        file = fopen("m.txt","w");
        printf("file created.\n");
    } else if(!strcmp(in[0],"-r")) {
        printf("trying to get in\n");
        sem = semget(KEY, 1, 0);
        shm = shmget(KEY, 1, 0);
        shmctl(shm, IPC_RMID, 0);
        printf("shared memory removed\n");
        semctl(sem, IPC_RMID, 0);
        rd();
        remove("m.txt");
        printf("file removed\nsemaphore removed\n");
    } else if(!strcmp(in[0],"-v")) {
        rd();
    } else {
        printf("Error : invalid input");
        exit(0);
    }
    return 0;
}
