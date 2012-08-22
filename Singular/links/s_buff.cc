#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include <coeffs/si_gmp.h>

#include <omalloc/omalloc.h>
#include <Singular/links/s_buff.h>
//struct s_buff_s
//{
//    char * buff; // buffer
//    int fd;      // file descrr.
//    int size;    // size of buff
//    int bp;      // current pos. in buff
//    int end;     // last position in buff
//};

//typedef struct s_buff_s * s_buff;

#define S_BUFF_LEN 8192

sigset_t ssi_sigmask; // set in ssiLink.cc
sigset_t ssi_oldmask; // set in ssiLink.cc

#define SSI_BLOCK_CHLD sigprocmask(SIG_SETMASK, &ssi_sigmask, &ssi_oldmask)
#define SSI_UNBLOCK_CHLD sigprocmask(SIG_SETMASK, &ssi_oldmask, NULL)

s_buff s_open(int fd)
{
  SSI_BLOCK_CHLD;
  s_buff F=(s_buff)omAlloc0(sizeof(*F));
  F->fd=fd;
  F->buff=(char*)omAlloc(S_BUFF_LEN);
  SSI_UNBLOCK_CHLD;
  return F;
}

s_buff s_open_by_name(const char *n)
{
  SSI_BLOCK_CHLD;
  int fd=open(n,O_RDONLY);
  SSI_UNBLOCK_CHLD;
  return s_open(fd);
}

int    s_close(s_buff &F)
{
  if (F!=NULL)
  {
    SSI_BLOCK_CHLD;
      #ifdef TEST_SBUFF
      printf("%d bytes read, %d read op.\n",F->len[10],F->len[0]);
      if (F->len[1]>0) printf("%d read op with <16 bytes\n",F->len[1]);
      if (F->len[2]>0) printf("%d read op with <32 bytes\n",F->len[2]);
      if (F->len[3]>0) printf("%d read op with <54 bytes\n",F->len[3]);
      if (F->len[4]>0) printf("%d read op with <128 bytes\n",F->len[4]);
      if (F->len[5]>0) printf("%d read op with <256 bytes\n",F->len[5]);
      if (F->len[6]>0) printf("%d read op with <512 bytes\n",F->len[6]);
      if (F->len[7]>0) printf("%d read op with <1024 bytes\n",F->len[7]);
      if (F->len[8]>0) printf("%d read op with <2048 bytes\n",F->len[8]);
      if (F->len[9]>0) printf("%d read op with >=2048 bytes\n",F->len[9]);
      #endif
    omFreeSize(F->buff,S_BUFF_LEN);
    int r=close(F->fd);
    omFreeSize(F,sizeof(*F));
    F=NULL;
    SSI_UNBLOCK_CHLD;
    return r;
  }
  return 0;
}

int s_getc(s_buff F)
{
  if (F==NULL)
  {
    printf("link closed");
    return 0;
  }
  if (F->bp>=F->end)
  {
    memset(F->buff,0,S_BUFF_LEN); /*debug*/
    SSI_BLOCK_CHLD;
    int r=read(F->fd,F->buff,S_BUFF_LEN);
    SSI_UNBLOCK_CHLD;
    if (r<=0)
    {
      F->is_eof=1;
      return -1;
    }
    else
    {
      F->end=r-1;
      F->bp=0;
      #ifdef TEST_SBUFF
      F->len[0]++;
      if (r<16) F->len[1]++;
      else if (r<32) F->len[2]++;
      else if (r<64) F->len[3]++;
      else if (r<128) F->len[4]++;
      else if (r<256) F->len[5]++;
      else if (r<512) F->len[6]++;
      else if (r<1024) F->len[7]++;
      else if (r<2048) F->len[8]++;
      else  F->len[9]++;
      F->len[10]+=r;
      #endif
      return F->buff[0];
    }
  }
  /*else*/
  F->bp++;
  return F->buff[F->bp];
}
int s_isready(s_buff F)
{
  if (F==NULL)
  {
    printf("link closed");
    return 0;
  }
  if (F->bp>=F->end) return 0;
  int p=F->bp+1;
  while((p<F->end)&&(F->buff[p]<=' ')) p++;
  if (p>=F->end) return 0;
  return 1;
}

void s_ungetc(int c, s_buff F)
{
  if (F==NULL)
  {
    printf("link closed");
    return;
  }
  if (F->bp>=0)
  {
    F->buff[F->bp]=c;
    F->bp--;
  }
}

int s_readint(s_buff F)
{
  if (F==NULL)
  {
    printf("link closed");
    return 0;
  }
  char c;
  int neg=1;
  int r=0;
  //int digit=0;
  do
  {
    c=s_getc(F);
  } while((!F->is_eof) && (c<=' '));
  if (c=='-') { neg=-1; c=s_getc(F); }
  while(isdigit(c))
  {
    //digit++;
    r=r*10+(c-'0');
    c=s_getc(F);
  }
  s_ungetc(c,F);
  //if (digit==0) { printf("unknown char %c(%d)\n",c,c); /*debug*/
  //                printf("buffer:%s\np=%d,e=%d\n",F->buff,F->bp,F->end);fflush(stdout); } /*debug*/
  return r*neg;
}

int s_readbytes(char *buff,int len, s_buff F)
{
  if (F==NULL)
  {
    printf("link closed");
    return 0;
  }
  int i=0;
  while((!F->is_eof)&&(i<len))
  {
    buff[i]=s_getc(F);
    i++;
  }
  return i;
}

void s_readmpz(s_buff F, mpz_t a)
{
  if (F==NULL)
  {
    printf("link closed");
    return;
  }
  mpz_set_ui(a,0);
  char c;
  int neg=1;
  do
  {
    c=s_getc(F);
  } while((!F->is_eof) && (c<=' '));
  if (c=='-') { neg=-1; c=s_getc(F); }
  while(isdigit(c))
  {
    mpz_mul_ui(a,a,10);
    mpz_add_ui(a,a,(c-'0'));
    c=s_getc(F);
  }
  s_ungetc(c,F);
  if (neg==-1) mpz_neg(a,a);
}

void s_readmpz_base(s_buff F, mpz_ptr a, int base)
{
  if (F==NULL)
  {
    printf("link closed");
    return;
  }
  mpz_set_ui(a,0);
  char c;
  int neg=1;
  do
  {
    c=s_getc(F);
  } while((!F->is_eof) && (c<=' '));
  if (c=='-') { neg=-1; c=s_getc(F); }
  while(c>' ')
  {
    if (isdigit(c))
    {
      mpz_mul_ui(a,a,base);
      mpz_add_ui(a,a,(c-'0'));
    }
    else if ((c>='a') && (c<='z'))
    {
      mpz_mul_ui(a,a,base);
      mpz_add_ui(a,a,(c-'a'+10));
    }
    else if ((c>='A') && (c<='Z'))
    {
      mpz_mul_ui(a,a,base);
      mpz_add_ui(a,a,(c-'A'+10));
    }
    else
    {
      s_ungetc(c,F);
      break;
    }
    c=s_getc(F);
  }
  if (neg==-1) mpz_neg(a,a);
}

int s_iseof(s_buff F)
{
  if (F!=NULL) return F->is_eof;
  else         return 1;
}

