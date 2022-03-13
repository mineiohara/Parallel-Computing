#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdbool.h>
#define N 8

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Using command: %s [Integer]\n", argv[0]);
        exit(1);
    }

    const int SIZE = N*sizeof(int);
    const char *shm_buffer_name = "shm_buffer";
    const char *shm_in_name = "shm_in";
    const char *shm_out_name = "shm_out";
    const char *shm_tag_name = "shm_tag";

    int shm_fd, shm_in, shm_out, shm_tag;
    static int *buffer, *in, *out;
    static bool *tag;

    shm_fd = shm_open(shm_buffer_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        printf("Shared memory failed\n");
        exit(-1);
    }

    shm_in = shm_open(shm_in_name, O_CREAT | O_RDWR, 0666);
    if (shm_in == -1)
    {
        printf("Shared memory failed\n");
        exit(-1);
    }

    shm_out = shm_open(shm_out_name, O_CREAT | O_RDWR, 0666);
    if (shm_out == -1)
    {
        printf("Shared memory failed\n");
        exit(-1);
    }

    shm_tag = shm_open(shm_tag_name, O_CREAT | O_RDWR, 0666);
    if (shm_tag == -1)
    {
        printf("Shared memory failed\n");
        exit(-1);
    }

    ftruncate(shm_fd, SIZE);
    ftruncate(shm_in, sizeof(int));
    ftruncate(shm_out, sizeof(int));
    ftruncate(shm_tag, sizeof(bool));

    buffer = (int*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (buffer == MAP_FAILED)
    {
        printf("Map buffer failed\n");
        return -1;
    }

    in = (int*)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_in, 0);
    if (in == MAP_FAILED)
    {
        printf("Map buffer failed\n");
        return -1;
    }

    out = (int*)mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_out, 0);
    if (out == MAP_FAILED)
    {
        printf("Map buffer failed\n");
        return -1;
    }

    tag = (bool*)mmap(0, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED, shm_tag, 0);
    if (tag == MAP_FAILED)
    {
        printf("Map buffer failed\n");
        return -1;
    }

    while(*in == *out && *tag)
    {
        printf("buffer in full!\n");
        // shm_unlink(shm_buffer_name);
        // shm_unlink(shm_in_name);
        // shm_unlink(shm_out_name);
        // shm_unlink(shm_tag_name);
        exit(1);
    }

    buffer[*in] = atoi(argv[1]);
    printf("Produce buffer[%d]: %d\n", *in, buffer[*in]);
    *in = (*in+ 1) % N;
    if (*in == *out) *tag = true;

    printf("Next in: %d, out: %d, tag: %d\n", *in, *out, *tag);

    // shm_unlink(shm_buffer_name);
    // shm_unlink(shm_in_name);
    // shm_unlink(shm_out_name);
    // shm_unlink(shm_tag_name);

    return 0;
}