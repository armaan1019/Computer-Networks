// COEN 146L : Lab 2 - template to use for creating multiple thread to make file transfers (step 1)
/*
* Name: Armaan Sharma
* Date: Jan 13, 2026
* Title: Lab2 – copying files using multiple threads
* Description: This program copies multiple files concurrently using multiple threads.
*/

#include <stdio.h>   // fprintf(), fread(), fwrite(), fopen(), fclose()
#include <stdlib.h>  // malloc(), free()
#include <pthread.h> // pthread_create()
#include <unistd.h>  // read(), write()
#include <fcntl.h>   // open(), close()
#include <errno.h>   // errno
#include <time.h>    // clock()

#define BUF_SIZE 2048 //buffer size

// data structure to hold copy
struct copy_struct {
	int thread_id; 
	char* src_filename;
	char* dst_filename; 
    double copy_time;
};

// copies a files from src_filename to dst_filename using functions fopen(), fread(), fwrite(), fclose()
int func_copy(char* src_filename, char* dst_filename) {
	FILE* src;
    FILE* dst;
    char* buf;
    size_t bytes_read;
	
	src = fopen(src_filename, "r");	// opens a file for reading
	if (src == NULL) { // fopen() error checking
		fprintf(stderr, "unable to open %s for reading: %i\n", src_filename, errno);
		exit(0);
	}
	dst = fopen(dst_filename, "w");	// opens a file for writing; erases old file/creates a new file
	if (dst == NULL) { // fopen() error checking
		fprintf(stderr, "unable to open/create %s for writing: %i\n", dst_filename, errno);
		exit(0);
	}

	buf = malloc((size_t)BUF_SIZE);  // allocate a buffer to store read data
	// reads content of file is loop iterations until end of file

	while ((bytes_read = fread(buf, 1, BUF_SIZE, src)) > 0) {
		fwrite(buf, 1, bytes_read, dst); // writes bytes_read to dst file
	}
	
	// closes src file pointer
	fclose(src);
	// closes dst file pointer
	fclose(dst);
	// frees memory used for buf
	free(buf);
	return 0;
}

// thread function to copy one file
void* copy_thread(void* arg) {
	struct copy_struct params = *(struct copy_struct*)arg;  // cast/dereference void* to copy_struct
    clock_t start = clock(); // start time measurement

	printf("thread[%i] - copying %s to %s\n", params.thread_id, params.src_filename, params.dst_filename);
	//call file copy function
	func_copy(params.src_filename, params.dst_filename);

    clock_t end = clock(); // end time measurement
    params.copy_time = ((double)(end - start)) / CLOCKS_PER_SEC; // calculate copy time in seconds
    printf("thread[%i] finished in %.6f seconds\n",
           params.thread_id,
           params.copy_time); // display copy time, note: fastest will be displayed first since threads run concurrently
	
	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	 // check correct usage of arguments in command line
	if (argc < 4) {  
		fprintf(stderr, "usage: %s <n numthreads> <src_file> <dst_file> ........\n", argv[0]);
		exit(1);
	}

	int num_threads = atoi(argv[1]); // number of threads from command line argument
	if(argc != (num_threads * 2) + 2) {
		fprintf(stderr, "usage: %s <n numthreads> <src_file> <dst_file> ........\n", argv[0]);
		exit(1);
	}

	char* src_filename;
	char* dst_filename;
	
	pthread_t threads[num_threads]; //initialize threads
	struct copy_struct thread_params[num_threads]; // structure for each thread
	int i;
	for (i = 0; i < num_threads; i++) {
		// initialize thread parameters
		thread_params[i].thread_id = i;
		thread_params[i].src_filename = argv[(i * 2) + 2]; // source file from command line argument
		thread_params[i].dst_filename = argv[(i * 2) + 3]; // destination file from command line argument
		// create each copy thread
		// use pthread_create(.....);
		pthread_create(&threads[i], NULL, copy_thread, &thread_params[i]); // get each thread, call copy thread, and pass parameters
	}

	// wait for all threads to finish
	for (i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
	}
	pthread_exit(NULL);
}