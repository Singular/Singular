/* trace-test.c
 * by Wolfram Gloger 1995
 */

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#ifdef sun
#include <sys/times.h>
#if !defined(HZ) && defined(CLK_TCK)
#define HZ CLK_TCK
#endif
#endif
#include <sys/resource.h>
#include <fcntl.h>

#include "malloc-trace.h"

#ifndef HAVE_MALLOPT
#define HAVE_MALLOPT 1 /* only makes sense for g++-malloc 2.6.2 or greater */
#endif
#ifndef BINS_MAX
#define BINS_MAX 50
#endif
#ifndef I_AVERAGE
#define I_AVERAGE	50
#endif

#define PAGE_MAX	16384
#if defined(__i386__) || defined(__sparc__) || defined(mips)
#define PAGE_SIZE 4096
#elif defined(__alpha__)
#define PAGE_SIZE 8192
#else
#error Define PAGE_SIZE !
#endif

/* The maximum amount of mem. allocated via sbrk().
 * Needed only for special allocators which use mmap().
 */
#define SBRK_MAX	(256UL*1024*1024)

#define HASH_N		997 /* prime */
#define HASH(id)	((id) % HASH_N)

#if HAVE_MALLOPT
int mallopt(int, int);
#endif

struct bin {
	unsigned long id;
	unsigned char *ptr;
	size_t size;
};

struct bin hash_table[HASH_N][BINS_MAX];

int test_flag=0, swap_flag=0, dist_flag=0, verb_level=1, time_flag=1;

int failures=0, allocs=0, frees=0, reallocs=0;
long sbrk_max=0;
unsigned long total_size, total_size_max=0, pages_in_use, max_request=0;
unsigned long total_size_cum=0;
unsigned long mmap_thresh=~0UL, mmap_size=0, mmap_regions=0, mmap_max=0;

unsigned char *base_ptr, *base_save;
unsigned short page_map[PAGE_MAX];

double mem_usage_sum, page_usage_sum, sbrk_used_sum, total_size_sum;
double mmap_size_sum;
double time_ini=0.0, time_prev=0.0, time_cur=0.0, time_sum;

unsigned long reqs[32] = {
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
};

void
classify_request(unsigned long size) {
  unsigned long b = 1;
  int i = 0;
  while (size > b) { ++i; b <<= 1; }
  reqs[i]++;
}

void
print_request_distribution() {
  int i;
  unsigned long b = 1;

  printf("# size distribution:\n");
  printf("#       bin        N\n");
  for (i = 0; i < 32; ++i) {
    if (reqs[i] != 0) 
      printf(" %10ld%10ld\n", b, reqs[i]);
    b <<= 1;
  }
}

struct bin *
find_free_bin(unsigned long id)
{
	struct bin *ptr;
	int i;

	if(!id) {
		printf("id=0 is not allowed!\n");
		exit(1);
	}
#ifdef DEBUG
	ptr = &hash_table[HASH(id)][0];
	for(i=BINS_MAX; i>0; i--, ptr++) {
		if(ptr->id == id) {
			printf("id %lx not unique!\n", id);
			exit(1);
		}
	}
#endif
	ptr = &hash_table[HASH(id)][0];
	for(i=BINS_MAX; i>0; i--, ptr++) {
		if(ptr->id == 0) {
			return ptr;
		}
	}
	printf("out of bins!\n");
	exit(1);
}

struct bin *
find_bin(unsigned long id)
{
	struct bin *ptr;
	unsigned int i;

	if(!id) return NULL;
	ptr = &hash_table[HASH(id)][0];
	for(i=BINS_MAX; i>0; i--, ptr++) {
		if(ptr->id == id) {
			return ptr;
		}
	}
	return NULL;
}

void
page_alloc(unsigned char *ptr, size_t size)
/* if ptr is NULL, the allocation is outside of sbrk()'ed memory */
{
	unsigned int p, q, pages;

	total_size += size;
	total_size_cum += size;
	if(total_size > total_size_max) total_size_max = total_size;
    if (size > max_request) max_request = size;
	if(!ptr || ptr<base_save || ptr>=(base_save+SBRK_MAX)) {
		if(++mmap_regions > mmap_max) mmap_max = mmap_regions;
		pages = size/PAGE_SIZE + 1;
		pages_in_use += pages;
		mmap_size += pages*PAGE_SIZE;
		return;
	}
	if(ptr < base_ptr) {
#ifdef VERBOSE
		printf("hmmm, allocating below brk...\n");
#endif
		base_ptr = ptr;
	}
	p = (unsigned long)(ptr - base_save)/PAGE_SIZE;
	q = (unsigned long)(ptr - base_save)%PAGE_SIZE + size;
	if(p >= PAGE_MAX) {
#if 1
		printf("page map overflow !\n");
		printf("p=%d max=%d\n", p, PAGE_MAX);
		exit(1);
#else
		return;
#endif
	}
	if(page_map[p++]++ == 0) pages_in_use++;
	while(q>PAGE_SIZE && p<PAGE_MAX) {
		if(page_map[p++]++ == 0) pages_in_use++;
		q -= PAGE_SIZE;
	}
}

void
page_free(unsigned char *ptr, size_t size)
{
	unsigned int p, q, pages;

	total_size -= size;
	if(!ptr || ptr<base_save || ptr>=(base_save+SBRK_MAX)) {
		--mmap_regions;
		pages = size/PAGE_SIZE + 1;
		pages_in_use -= pages;
		mmap_size -= pages*PAGE_SIZE;
		return;
	}
	p = (unsigned long)(ptr - base_save)/PAGE_SIZE;
	q = (unsigned long)(ptr - base_save)%PAGE_SIZE + size;
	if(p >= PAGE_MAX) {
#if 1
		printf("page map overflow !\n");
		printf("p=%d max=%d\n", p, PAGE_MAX);
		exit(1);
#else
		return;
#endif
	}
	if(--page_map[p++] == 0) pages_in_use--;
	while(q>PAGE_SIZE && p<PAGE_MAX) {
		if(--page_map[p++] == 0) pages_in_use--;
		q -= PAGE_SIZE;
	}
}

void
bin_alloc(struct bin *m, size_t size, unsigned long id)
{
	m->id = id;
	m->size = size;
	if(dist_flag) classify_request(size);
	if(m->size < mmap_thresh) {
		m->ptr = (unsigned char *)malloc(m->size);
		if(!m->ptr) {
			printf("out of memory!\n");
			exit(1);
		}
		if(test_flag && m->size>0) {
			/* test read/write */
			m->ptr[0] = ((unsigned)m->ptr ^ m->size) & 0xFF;
			m->ptr[m->size-1] = m->ptr[0];
		}
	} else {
		m->ptr = 0;
	}
	page_alloc(m->ptr, m->size);
}

void
bin_realloc(struct bin *m, size_t size, unsigned long newid)
{
	struct bin *newptr;

	if(test_flag && m->ptr && m->size>0 &&
	   (m->ptr[0] != (((unsigned)m->ptr ^ m->size) & 0xFF) ||
		m->ptr[m->size-1] != m->ptr[0])) {
		printf("memory corrupt!\n");
		exit(1);
	}
	page_free(m->ptr, m->size);
	if(dist_flag) classify_request(size);
	if(m->ptr) {
		m->ptr = (unsigned char *)realloc(m->ptr, size);
		m->size = size;
		if(!m->ptr) {
			printf("out of memory!\n");
			exit(1);
		}
	} else {
		if(size < mmap_thresh) {
			m->ptr = (unsigned char *)malloc(m->size);
			if(!m->ptr) {
				printf("out of memory!\n");
				exit(1);
			}
		} else {
			m->size = size;
		}
	}
	page_alloc(m->ptr, m->size);
	if(test_flag && m->ptr && m->size>0) {
		/* test read/write */
		m->ptr[0] = ((unsigned)m->ptr ^ m->size) & 0xFF;
		m->ptr[m->size-1] = m->ptr[0];
	}
	if(newid != m->id) {
		newptr = find_free_bin(newid);
		newptr->id = newid;
		newptr->ptr = m->ptr;
		newptr->size = size;
		m->id = 0;
	}
}

void
bin_free(struct bin *m)
{
	if(test_flag && m->ptr && m->size>0 &&
	   (m->ptr[0] != (((unsigned)m->ptr ^ m->size) & 0xFF) ||
	    m->ptr[m->size-1] != m->ptr[0])) {
		printf("memory corrupt!\n");
		exit(1);
	}
	page_free(m->ptr, m->size);
	if(m->ptr) free(m->ptr);
	m->id = 0;
}

void
get_resources(int i)
/* 0: start, 1: stop, other: print */
{
	static double utime = 0.0, stime = 0.0;
#if !defined(sun)
	static struct rusage ru0, ru1;
	static long min_flt = 0, maj_flt = 0;

	switch(i) {
	case 0:
		getrusage(RUSAGE_SELF, &ru0);
		return;
	case 1:
		getrusage(RUSAGE_SELF, &ru1);
		utime += ((double)ru1.ru_utime.tv_sec-(double)ru0.ru_utime.tv_sec) +
		 ((double)ru1.ru_utime.tv_usec-(double)ru0.ru_utime.tv_usec)*1.0E-6;
		stime += ((double)ru1.ru_stime.tv_sec-(double)ru0.ru_stime.tv_sec) +
		 ((double)ru1.ru_stime.tv_usec-(double)ru0.ru_stime.tv_usec)*1.0E-6;
		min_flt += ru1.ru_minflt - ru0.ru_minflt;
		maj_flt += ru1.ru_majflt - ru0.ru_majflt;
		return;
	}
	printf("# minor pagefaults      : %10ld\n", min_flt);
	printf("# major pagefaults      : %10ld\n", maj_flt);
#else
	static struct tms t0, t1;
	switch(i) {
	case 0:
		times(&t0);
		return;
	case 1:
		times(&t1);
		utime += (double)(t1.tms_utime-t0.tms_utime)/HZ;
		stime += (double)(t1.tms_stime-t0.tms_stime)/HZ;
		return;
	}
#endif
	printf("# user time             : %10.3f sec\n", utime);
	printf("# system time           : %10.3f sec\n", stime);
}

void
swap_long(unsigned long *ptr, int count)
{
	union {
		unsigned char uc[4];
		unsigned long ul;
	} u1, u2;

	while(count-- > 0) {
		u1.ul = *ptr;
		u2.uc[0] = u1.uc[3];
		u2.uc[1] = u1.uc[2];
		u2.uc[2] = u1.uc[1];
		u2.uc[3] = u1.uc[0];
		*ptr++ = u2.ul;
	}
}

int
test_file(int fd)
{
	int i, j, next_i, a_n, count;
	long sum;
	ACTION a_buf[256], *a;
	struct bin *bptr;

	/* Free the hash table. */
	for(bptr = &hash_table[0][0],
		sum = sizeof(hash_table)/sizeof(hash_table[0][0]);
		sum>0; bptr++, sum--) {
		if(bptr->id) {
			if(bptr->ptr) free(bptr->ptr);
			bptr->id = 0;
		}
	}
	/* Clear the page map. */
	for(i=0; i<PAGE_MAX; i++) page_map[i] = 0;
	pages_in_use = total_size = mmap_size = 0;

	/* Start the `stop watch'. */
	get_resources(0);

	/* Print a title bar */
	if(verb_level & (0x02|0x04)) {
		printf("#");
		if(verb_level & 0x02) {
			if(time_flag) printf("        time");
			else printf("      i");
		}
		if(verb_level & 0x04) {
			printf(" size_cum");
		}
		printf("    total     sbrk");
		if(mmap_thresh != ~0UL) printf("     mmap");
		printf(" mem_us page_u\n");
	}

	for(i=next_i=a_n=count=0, a=NULL;;) {
		if(--a_n <= 0) {
			a_n = read(fd, a_buf, sizeof(a_buf))/sizeof(a_buf[0]);
			if(a_n <= 0) break;
			if(swap_flag) swap_long((unsigned long *)a_buf, 4*a_n);
			a = a_buf;
		} else a++;
		switch(a->code) {
		case CODE_MALLOC: case CODE_CALLOC: case CODE_MEMALIGN:
            ++allocs;
			bptr = find_free_bin((unsigned long)a->ptr);
			bin_alloc(bptr, a->size, (unsigned long)a->ptr);
			++i;
			break;
		case CODE_FREE: case CODE_CFREE:
            ++frees;
			if(!a->ptr) { /* free(0) has no effect, but warn about it... */
#ifdef VERBOSE
				printf("warning: free(0) called\n");
#endif
			} else {
				bptr = find_bin((unsigned long)a->ptr);
				if(!bptr) {
					if(failures++ < 10)
					 printf("free: can't find bin for ptr=%x, ignoring\n",
							(unsigned)a->ptr);
				} else bin_free(bptr);
			}
			++i;
			break;
		case CODE_REALLOC:
			++reallocs;
			if(!a->ptr) { /* realloc(0, size) is the same as malloc(size) */
				bptr = find_free_bin((unsigned long)a->ptr2);
				bin_alloc(bptr, a->size, (unsigned long)a->ptr2);
			} else {
				bptr = find_bin((unsigned long)a->ptr);
				if(!bptr) {
					if(failures++ < 10)
					 printf("realloc: can't find bin for ptr=%x, ignoring\n",
							(unsigned)a->ptr);
				} else bin_realloc(bptr, a->size, (unsigned long)a->ptr2);
			}
			++i;
			break;
		case CODE_TIMESTAMP:
			time_cur = (double)a->size + (double)((long)a->ptr)*1.0E-6;
			break;
		default:
			printf("warning: code %d is unknown\n", a->code);
			continue;
		}
		if(i == 0) { /* first action is time stamp */
			time_ini = time_prev = time_cur;
			if(verb_level & (0x02|0x04)) {
				time_t t = (time_t)a->size;
				printf("#  start time: %s", ctime(&t));
			}
		} else { /* chances are that this is an old trace file */
			time_flag = 0;
		}
		if(time_flag ? (a->code == CODE_TIMESTAMP) : (i >= next_i)) {
			/* gather statistics */
			double dt;

			++count;
			if(time_flag) {
				dt = time_cur - time_prev;
#if 0
				/* maybe use an upper threshold here ? */
				if(dt > 5.0) dt = 5.0;
#endif
			} else {
				dt = 1.0;
			}
			sum = (long)sbrk(0) - (long)base_ptr;
			if(dt<0.0 || sum<0) {
				printf("This can't happen.\n");
				exit(1);
			}
			if(sum > sbrk_max) sbrk_max = sum;
			sbrk_used_sum += dt*sum;
			total_size_sum += dt*(double)total_size;
			mmap_size_sum += dt*(double)mmap_size;
			mem_usage_sum += dt*((sum+mmap_size > 0) ?
			 (double)total_size/(double)(sum+mmap_size) : 1.0);
			page_usage_sum += dt*((pages_in_use > 0) ?
			 (double)total_size/(pages_in_use*PAGE_SIZE) : 1.0);
			time_sum += dt;
			time_prev = time_cur;
			next_i += I_AVERAGE;
			if(verb_level & (0x02|0x04)) {
				if(verb_level & 0x02) {
					if(time_flag) printf(" %12.6f", time_cur-time_ini);
					else printf("%8d", i);
				}
				if(verb_level & 0x04) {
					printf(" %8ld", total_size_cum);
				}
				printf(" %8ld %8ld", total_size, sum);
				if(mmap_thresh != ~0UL) printf(" %8ld", mmap_size);
				printf(" %6.2f %6.2f\n", ((sum+mmap_size > 0) ?
					   (double)total_size/(double)(sum+mmap_size) : 1.0)*100.0,
					   ((pages_in_use > 0) ?
					   (double)total_size/(pages_in_use*PAGE_SIZE) : 1.0)*100.0);
			}
		}
	}

	/* Stop the `stop watch'. */
	get_resources(1);

#if 0
	for(j=PAGE_MAX-1; j>0 && page_map[j]==0; j--);
	for(i=0; i<=j; i++) printf("%3hu ", page_map[i]);
	printf("\n");
#endif
	/* Consistency check. */
	for(i=j=0, sum=0; i<PAGE_MAX; i++) {
		if(page_map[i] > 0) {
			sum += page_map[i];
			j++;
		}
	}
	if(j != (pages_in_use - mmap_size/PAGE_SIZE)) {
		printf("bug!\n");
		exit(1);
	}

	return count;
}

int
main(int argc, char *argv[])
{
	int count, fd;
	struct { unsigned long version, initial_brk; } header;
	char *cptr;
	unsigned long ul;

	/* Try to estimate the lowest possible address. */
	base_ptr = (unsigned char *)sbrk(0);
	base_save = (unsigned char *)(((unsigned long)base_ptr >> 24) << 24);

	/* Don't use getopt - it seems to use malloc(). */
	while(--argc>0 && (*++argv)[0]=='-') switch((*argv)[1]) {
	case 't':
		test_flag = 1;
		break;
	case 's':
		swap_flag = 1;
		break;
	case 'd':
		dist_flag = 1;
		break;
	case 'v':
		verb_level = atoi(&(*argv)[2]);
		break;
	case 'T':
		time_flag ^= 1;
		break;
	case 'm':
		mmap_thresh = strtoul(&(*argv)[2], &cptr, 10);
		if(mmap_thresh == 0) {
			printf("invalid -m threshold\n");
			return -1;
		}
		if(*cptr=='k' || *cptr=='K') mmap_thresh *= 1024UL;
		/*printf("setting mmap_thresh to %lu\n", mmap_thresh);*/
		break;
#if HAVE_MALLOPT
	case 'M':
		ul = strtoul(&(*argv)[2], &cptr, 10);
		if(ul == 0) {
			printf("invalid -M threshold\n");
			return -1;
		}
		if(*cptr=='k' || *cptr=='K') ul *= 1024UL;
		if(!mallopt(-3, (int)ul)) printf("mallopt(%d,...) failed\n", -3);
		break;
	case 'a':
		ul = strtoul(&(*argv)[2], &cptr, 10);
		if(ul == 0) {
			printf("invalid -a threshold\n");
			return -1;
		}
		if(*cptr=='k' || *cptr=='K') ul *= 1024UL;
		mallopt(-1, (int)ul);
		/*printf("setting trim thresh to %lu\n", ul);*/
		break;
#endif
	default:
		printf("Unknown option `%s' ignored.\n", &(*argv)[1]);
	}
	if(argc <= 0) {
		printf("Usage: trace-test [-t] [-s] [-d] [-v<level>] [-T]"
			   "[-m<thresh>] <trace-file>\n");
		printf(" -t: test memory\n");
		printf(" -s: swap words (to read file with opposite endianness)\n");
		printf(" -d: print size distribution\n");
		printf(" -v: verbosity level (default: 1)\n");
		printf(" -T: toggle time weighting (default: enabled)\n");
		printf(" -m<thresh>: set mmap()'ing threshold (e.g. '64k')\n");
		return -1;
	}
	mem_usage_sum = page_usage_sum = sbrk_used_sum = total_size_sum =
	 mmap_size_sum = time_sum = 0.0;
	count = 0;
	while(argc-- > 0) {
		fd = open(*argv, O_RDONLY);
		if(fd < 0) {
			printf("can't open %s\n", *argv);
			continue;
		}
		read(fd, &header, sizeof(header));
		if(verb_level & (0x02|0x04)) printf("# trace file: %s\n", *argv);
		argv++;
		if(swap_flag)
		 swap_long((unsigned long *)&header, sizeof(header)/sizeof(long));
		if(header.version <= CODE_LAST) {
			/* Be backward compatible. */
			lseek(fd, 0, SEEK_SET);
		} else if(count == 0) {
			/* Adjust brk such that it is exactly as it was
			 * originally (modulo the page size).
			 */
			size_t b0, b1;
			ptrdiff_t correction;

			b0 = (size_t)sbrk(0) & (PAGE_SIZE-1);
			b1 = header.initial_brk & (PAGE_SIZE-1);
			correction = b0>b1 ? (PAGE_SIZE - (b0-b1)) : b1-b0;
			/*printf("correcting brk by %ld bytes\n", (long)correction);*/
			sbrk(correction);
			base_ptr = (unsigned char *)sbrk(0);
		}
		count += test_file(fd);
		close(fd);
	}

	if(count <= 0) return 0;

#ifdef DEBUG
	malloc_stats();
#endif

	if(verb_level == 0) { /* special terse output */
		printf("%5.2f %5.2f %3ld",
			   (1.0 - mem_usage_sum/time_sum)*100.0,
			   page_usage_sum/time_sum*100.0, mmap_max);
		return 0;
	}
	/* First print statistics about the allocation pattern. */
	printf("# Stats:\n");
	printf("# number of ops         : %10d\n", failures+allocs+frees+reallocs);
	if(failures > 0)
	printf("# number of failures    : %10d\n", failures);
	printf("# number of allocs      : %10d\n", allocs);
	printf("# number of frees       : %10d\n", frees);
	printf("# number of reallocs    : %10d\n", reallocs);
	printf("# current size allocated: %10ld bytes\n", total_size);
	printf("# maximum size allocated: %10ld bytes\n", total_size_max);
	printf("# average size allocated: %10ld bytes\n",
		   (long)(total_size_sum/time_sum));
	printf("# maximum request size  : %10ld bytes\n", max_request);
	if(dist_flag) print_request_distribution();
	if(time_flag)
	printf("# time elapsed          : %10.4f sec\n", time_cur-time_ini);

	/* Now data for performance evaluation. */
	printf("# Performance:\n");
	get_resources(2);
	printf("# max. sbrk()'ed memory : %10ld bytes\n", sbrk_max);
	printf("# avg. sbrk()'ed memory : %10ld bytes\n",
		   (long)(sbrk_used_sum/time_sum));
	if(mmap_size_sum > 0.0) {
		printf("# max. mmap()'ed regions: %10ld\n", mmap_max);
		printf("# avg. mmap()'ed memory : %10ld bytes\n",
			   (long)(mmap_size_sum/time_sum));
	}
	printf("# average heap/mem waste: %10.2f %%\n",
		   (1.0 - mem_usage_sum/time_sum)*100.0);
	printf("# average page usage    : %10.2f %%\n",
		   page_usage_sum/time_sum*100.0);

	if(verb_level & 0x20) {
		/* This is not so interesting... */
		printf("# current page usage: %.2f%%\n",
			   (double)total_size/(pages_in_use*PAGE_SIZE)*100.0);
		printf("# initial brk: %lx\n", (long)base_ptr);
	}

	return 0;
}

/*
 * Local variables:
 * tab-width: 4
 * compile-command: "gcc -Wall trace-test.c -o trace-test"
 * End:
 */

