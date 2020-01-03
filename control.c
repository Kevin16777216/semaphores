#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define KEY 24602
#define SIZE 200
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};
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
    FILE *f;
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
    if(!strcmp(in[0],"-c")) {
        int semd,shmd,r,v;
        char* data;
        semd = semget(KEY, 1, IPC_CREAT | IPC_EXCL | 0644);
        if (semd == -1) {
            semd = semget(KEY, 1, 0);
            v = semctl(semd, 0, GETVAL, 0);
            printf("Semaphore already exists.\n");
        } else {
            union semun us;
            us.val = 1;
            r = semctl(semd, 0, SETVAL, us);
            printf("semaphore created.\n");
        }
        shmd = shmget(KEY, SIZE, IPC_CREAT | 0644);
        printf("shared memory created.\n");
        data = shmat(shmd, 0, 0);
        f = fopen("m.txt","w");
        printf("file created.\n");

    } else if(!strcmp(in[0],"-r")) {
        printf("trying to get in\n");
        int semd = semget(KEY, 1, 0);
        int shmd = shmget(KEY, 1, 0);
        shmctl(shmd, IPC_RMID, 0);
        printf("shared memory removed\n");
        semctl(semd, IPC_RMID, 0);
        f = fopen("m.txt", "r");
        char line[SIZE];
        while(fgets(line, SIZE, f)) {
            printf("%s", line);
        }
        remove("m.txt");
        printf("file removed.\nsemaphore removed.\n");
    } else if(!strcmp(in[0],"-v")) {
        f = fopen("m.txt", "r");
        char line[SIZE];
        while(fgets(line, SIZE, f)) {
            printf("%s", line);
        }
    } else {
        printf("Error : invalid input");
        exit(0);
    }
    return 0;
}
