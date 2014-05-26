#ifndef S_BUFFIO_H
#define S_BUFFIO_H

#include<signal.h>
#include<gmp.h>

struct s_buff_s
{
    char * buff; // buffer
    int fd;      // file descrr.
    int bp;      // current pos. in buff (of the last read char)
    int end;     // last position in buff
    int is_eof;
};

typedef struct s_buff_s * s_buff;

s_buff s_open(int fd);
s_buff s_open_by_name(const char *n);
int    s_free(s_buff &f);
int    s_close(s_buff &f);

int s_getc(s_buff F);
void s_ungetc(int c, s_buff F);

int s_readint(s_buff F);
long s_readlong(s_buff F);
int s_readbytes(char *buff,int len, s_buff F);
void s_readmpz(s_buff F, mpz_ptr a);
void s_readmpz_base(s_buff F, mpz_ptr a, int base);
int s_isready(s_buff F);
int s_iseof(s_buff F);
#endif
