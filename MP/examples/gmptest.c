/*
  gmptest.c - Test implementation of arbitrary precision integer package.
	      Read signed and unsigned ApInt integers from standard input,
	      send them to a file in an internal format, read them back
	      and display them to standard output.  Perform conversions
	      from ApInt to 32 bit integer and vice versa.
*/

#include <stdio.h>
/*#include "MP.h"*/
#include  "gmp.h" 

#define RawFile "rawt"

main()
{
  MP_INT n;
  char s[100];
  long x;
  char *sint = NULL;
  unsigned long int y;
  FILE *fd;

  /* We have to initialize the arbitrary integer before using it */
  mpz_init(&n);
  fd = fopen(RawFile,"w");
  printf("enter an arbitrary precision integer for n ");
  mpz_inp_str(&n, stdin, 0);
  printf("the number read is ");
  mpz_out_str(stdout, 10, &n);printf("\n");
  mpz_out_raw(fd, &n);
  printf("The number has been stored to file %s in raw format\n\n", RawFile);
  fclose(fd);
  printf("alloc = %d, size = %d\n", n._mp_alloc, n._mp_size);
  fd = fopen(RawFile,"r");
  sint = mpz_get_str(sint, 10, &n);
  printf("sint = %s\n", sint);
  mpz_inp_raw(&n, fd); fclose(fd);
  printf("The arbitrary precision retrieved from %s is ", RawFile);fflush(stdout);
  mpz_out_str(stdout, 10, &n);printf("\n\n"); 
	
  /* free and initialize the arbitrary integer n for the next use */
  printf("Freeing and initializing arbitrary integer n for reuse\n");
  mpz_clear(&n);
  mpz_init(&n);
  printf("Enter signed 32 bit value for x ");
  scanf("%d",&x);
  mpz_init_set_si(&n, x);
  printf("The arbitrary precision integer n after mpz_init_set_si is ");
  fflush(stdout);
  mpz_out_str(stdout, 10, &n);printf("\n\n");
 
  x  = mpz_get_si(&n);
  printf("mpz_get_se returns signed 32 bit int %d\n",x);
  printf("The arbitrary precision integer n after mpz_get_si is ");
  mpz_out_str(stdout, 10, &n);printf("\n\n");

  printf("Freeing and initializing arbitrary integer n for reuse\n");
  mpz_clear(&n);
  mpz_init(&n);
  printf("Enter unsigned 32 bit integer value for y ");
  scanf("%u",&y);
  mpz_init_set_ui(&n,y);
  printf("After mpz_init_set_ui() arbitrary precision integer n is ");
  mpz_out_str(stdout, 10, &n);printf("\n"); y = 0;

  y = mpz_get_ui(&n);
  printf("mpz_get_ui() returns unsigned 32 bit int %u\n", y);

  /* cleanup after ourselves */
  unlink (RawFile); 
  mpz_clear(&n);

}
