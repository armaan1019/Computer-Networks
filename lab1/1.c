// Name: Armaan Sharma
// Date: Jan 6, 2025
// Title: Lab 1 - copying files using functions and system calls
// Description: This program runs two different ways to copy a file sequentially and compares their runtimes.

#include <stdio.h>   // fprintf(), fread(), fwrite(), fopen(), fclose()
#include <stdlib.h>  // malloc(), free()
#include <pthread.h> // pthread_create()
#include <unistd.h>  // read(), write()
#include <fcntl.h>   // open(), close()
#include <errno.h>   // errno
#include <time.h>    // clock()

#define BUF_SIZE 2048 // buffer size

// STEP 1: Copies a file from src_filename to dst_filename using C library functions
int func_copy(char* src_filename, char* dst_filename) {

    FILE *src, *dst; // source and destination file pointers
    char *buf; // buffer to store read data
    size_t bytes_read; // number of bytes read, never negative

    src = fopen(src_filename, "r"); // open file for reading
    if (src == NULL) {
        fprintf(stderr, "unable to open %s for reading: %i\n", src_filename, errno);
        exit(1);
    }

    dst = fopen(dst_filename, "w"); // open file for writing
    if (dst == NULL) {
        fprintf(stderr, "unable to open/create %s for writing: %i\n", dst_filename, errno);
        fclose(src);
        exit(1);
    }

    buf = malloc((size_t)BUF_SIZE); // allocate buffer to store read data
    if (buf == NULL) {
        fprintf(stderr, "buffer allocation failed\n");
        fclose(src);
        fclose(dst);
        exit(1);
    }

    // read/write loop until EOF
    while ((bytes_read = fread(buf, 1, BUF_SIZE, src)) > 0) { // fread is going to return 0 or value > 0 which will
        fwrite(buf, 1, bytes_read, dst); // be the bytes read. bytes_read is used to write only the valid bytes.
    }

    fclose(src);
    fclose(dst);
    free(buf);

    return 0;
}

// STEP 2:Copies a file using UNIX system calls
int syscall_copy(char* src_filename, char* dst_filename) {

    int src_fd, dst_fd;
    char *buf;
    ssize_t bytes_read; // subtle difference from before, bytes_read can be -1 on error

    src_fd = open(src_filename, O_RDONLY); // Open file for reading
    if (src_fd < 0) {
        fprintf(stderr, "unable to open %s for reading: %i\n", src_filename, errno);
        exit(1);
    }

    dst_fd = open(dst_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644); // Open file for writing, create if it doesn't exist, truncate if it does
    if (dst_fd < 0) {
        fprintf(stderr, "unable to open/create %s for writing: %i\n", dst_filename, errno);
        close(src_fd);
        exit(1);
    }

    buf = malloc((size_t)BUF_SIZE); // allocate buffer to store read data
    if (buf == NULL) {
        fprintf(stderr, "buffer allocation failed\n");
        close(src_fd);
        close(dst_fd);
        exit(1);
    }

    // read/write loop
    while ((bytes_read = read(src_fd, buf, BUF_SIZE)) > 0) { // Same concept as before
        write(dst_fd, buf, bytes_read); // read tries to read up to BUF_SIZE bytes, returns actual bytes read
    }

    close(src_fd);
    close(dst_fd);
    free(buf);

    return 0;
}

// STEP 3: Compare runtime between function copy and syscall copy
void check_copy_times(char* src_filename, char* dst_filename) {

    clock_t func_start, func_end, syscall_start, syscall_end;
    double func_time, syscall_time;

    func_start = clock(); // start time measurement
    func_copy(src_filename, dst_filename); // perform copy using functions
    func_end = clock(); // end time measurement

    func_time = (double)(func_end - func_start) / CLOCKS_PER_SEC; // calculate time in seconds

    syscall_start = clock();
    syscall_copy(src_filename, dst_filename);
    syscall_end = clock();

    syscall_time = (double)(syscall_end - syscall_start) / CLOCKS_PER_SEC; // Same thing but for system calls

    printf("time to copy using functions: %.7f seconds\n", func_time);
    printf("time to copy using syscalls: %.7f seconds\n", syscall_time);
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        fprintf(stderr, "usage: %s <src_filename> <dst_filename>\n", argv[0]);
        exit(1);
    }

    check_copy_times(argv[1], argv[2]);

    return 0;
}