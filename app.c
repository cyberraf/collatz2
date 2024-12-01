#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

void collatz(int n, int *shared_mem);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <positive integer>\n", argv[0]);
        exit(1);
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        printf("%s: error: argument must be a positive integer\n", argv[0]);
        exit(2);
    }

    // Establish shared memory
    int *shared_mem;
    size_t shared_mem_size = sizeof(int) * 1024;
    int shared_mem_fd = shm_open("/collatz_shared_mem", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shared_mem_fd == -1) {
        printf("%s: error: shm_open failed\n", argv[0]);
        exit(3);
    }
    if (ftruncate(shared_mem_fd, shared_mem_size) == -1) {
        printf("%s: error: ftruncate failed\n", argv[0]);
        exit(4);
    }
    shared_mem = mmap(NULL, shared_mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_mem_fd, 0);
    if (shared_mem == MAP_FAILED) {
        printf("%s: error: mmap failed\n", argv[0]);
        exit(5);
    }

    pid_t pid = fork();
    if (pid < 0) {
        printf("%s: error: fork failed\n", argv[0]);
        exit(6);
    } else if (pid == 0) {
        // Child process
        collatz(n, shared_mem);
        exit(EXIT_SUCCESS);
    } else {
        // Parent process
        wait(NULL);
        for (int i = 0; i < 1024; i++) {
            if (shared_mem[i] == 0) {
                break;
            }
            printf("%d ", shared_mem[i]);
        }
        printf("\n");

        // Remove shared memory object
        munmap(shared_mem, shared_mem_size);
        shm_unlink("/collatz_shared_mem");
    }

    return 0;
}

void collatz(int n, int *shared_mem) {
    int i = 0;
    shared_mem[i++] = n;
    while (n != 1) {
        if (n % 2 == 0) {
            n = n / 2;
        } else {
            n = 3 * n + 1;
        }
        shared_mem[i++] = n;
    }
    shared_mem[i] = 0; // Mark end of sequence
}
