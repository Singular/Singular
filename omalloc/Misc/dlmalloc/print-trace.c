/*
 * print-trace.c
 *
 * by Wolfram Gloger 1995
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "malloc-trace.h"

int main(int argc, char *argv[])
{
    ACTION a;
    int fd;
    long mallocs=0, frees=0, reallocs=0;
    struct { unsigned version, initial_brk; } header;
    double t0, t1, dt = 0.0;

    if(argc != 2) {
	fprintf(stderr, "usage: %s file\n", argv[0]);
	return -1;
    }
    fd = open(argv[1], O_RDONLY);
    if(fd < 0) {
	fprintf(stderr, "can't open %s\n", argv[1]);
    }
    read(fd, &header, sizeof(header));
    if(header.version < CODE_LAST) {
	lseek(fd, 0, SEEK_SET);
    } else {
	printf("Mtrace version: %x\n", header.version);
	printf("Initial brk: %x\n", header.initial_brk);
    }
    while(read(fd, &a, sizeof(a)) == sizeof(a)) {
	switch(a.code) {
	case CODE_MALLOC:
	    printf("malloc   %8p %8ld\n", a.ptr, (long)a.size);
	    mallocs++;
	    break;
	case CODE_REALLOC:
	    printf("realloc  %8p %8ld %p\n", a.ptr, (long)a.size, a.ptr2);
	    reallocs++;
	    break;
	case CODE_CALLOC:
	    printf("calloc   %8p %8ld\n", a.ptr, (long)a.size);
	    mallocs++;
	    break;
	case CODE_MEMALIGN:
	    printf("memalign %8p %ld %8ld\n", a.ptr, (long)a.size, (long)a.ptr2);
	    mallocs++;
	    break;
	case CODE_FREE:
	    printf("free     %8p\n", a.ptr);
	    frees++;
	    break;
	case CODE_CFREE:
	    printf("cfree    %8p\n", a.ptr);
	    frees++;
	    break;
	case CODE_TIMESTAMP:
	    t0 = (double)a.size + (double)((long)a.ptr)*1.0E-6;
	    if(dt == 0.0) {
		time_t t = (time_t)a.size;
		printf("start    %s", ctime(&t));
		t1 = t0;
	    } else printf("time     %.6f\n", t0-dt);
	    dt = t0;
	    break;
	default:
	    fprintf(stderr, "Unknown code (%d), exiting.\n", a.code);
	    close(fd);
	    return -2;
	}
    }
    close(fd);
    printf("Stats: allocs=%ld reallocs=%ld frees=%ld\n",
	   mallocs, reallocs, frees);
    printf("Elapsed time: %.6f sec\n", t0-t1);
    return 0;
}
