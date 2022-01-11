/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: input from ttys, simulating fgets
*/





#include "kernel/mod2.h"
#include "omalloc/omalloc.h"
#include "kernel/oswrapper/feread.h"

#ifdef HAVE_FEREAD
  #include <unistd.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <sys/time.h>
  #include <sys/types.h>
  #include <string.h>

  #if 0
    #include <pc.h>
  #else
    #ifdef SunOS_5
      /* solaris special, found with v 5.7 */
      #define _XOPEN_SOURCE_EXTENDED
      #include "/usr/xpg4/include/term.h"
    #endif
    #if 0
      #ifndef SunOS_5
        #include <term.h>
      #endif
    #elif HAVE_TERMCAP_H
      #ifndef SunOS_5
      #include <termcap.h>
      #endif
    #endif
    #if defined(HAVE_TERMIOS_H) && ! defined(TCSANOW)
      #include <termios.h>
    #endif
    #if defined(HAVE_TERM_H) && ! defined(TCSANOW)
      #include <term.h>
    #endif

  #endif


#ifndef STDIN_FILENO
  #define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
  #define STDOUT_FILENO 1
#endif

#define feCTRL(C) ((C) & 0x1F)    /* <ctrl> character  */

VAR struct termios fe_saved_attributes;

STATIC_VAR BOOLEAN fe_stdout_is_tty;
STATIC_VAR BOOLEAN fe_stdin_is_tty;
VAR BOOLEAN fe_use_fgets=FALSE;
STATIC_VAR BOOLEAN fe_is_initialized=FALSE;

VAR FILE *  fe_echo; /*the output file for echoed characters*/

#define fe_hist_max 32
VAR char ** fe_hist=NULL;
VAR short   fe_hist_pos;
VAR BOOLEAN fe_is_raw_tty=0;
VAR short   fe_cursor_pos; /* 0..colmax-1*/
VAR short   fe_cursor_line; /* 0..pagelength-1*/

#ifndef HAVE_ATEXIT
  int on_exit(void (*f)(int, void *), void *arg);
  #ifdef HAVE_FEREAD
    void fe_reset_fe (int i, void *v)
  #endif
#else
  #ifdef HAVE_FEREAD
    void fe_reset_fe (void)
  #endif
#endif
{
  if (fe_stdin_is_tty)
  {
    int i;
    if (fe_is_raw_tty)
    {
      tcsetattr (STDIN_FILENO, TCSANOW, &fe_saved_attributes);
      fe_is_raw_tty=0;
    }
    if (fe_hist!=NULL)
    {
      for(i=fe_hist_max-1;i>=0;i--)
      {
        if (fe_hist[i] != NULL) omFree((ADDRESS)fe_hist[i]);
      }
      omFreeSize((ADDRESS)fe_hist,fe_hist_max*sizeof(char *));
      fe_hist=NULL;
    }
    if (!fe_stdout_is_tty)
    {
      fclose(fe_echo);
    }
  }
}
void fe_temp_reset (void)
{
  if (fe_is_raw_tty)
  {
    tcsetattr (STDIN_FILENO, TCSANOW, &fe_saved_attributes);
    fe_is_raw_tty=0;
  }
}
void fe_temp_set (void)
{
  if(fe_is_raw_tty==0)
  {
    struct termios tattr;

    /* Set the funny terminal modes. */
    tcgetattr (STDIN_FILENO, &tattr);
    tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
    tattr.c_cc[VMIN] = 1;
    tattr.c_cc[VTIME] = 0;
    tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
    fe_is_raw_tty=1;
  }
}

STATIC_VAR char termcap_buff[2048];
static int fe_out_char(int c)
{
  fputc(c,fe_echo);
  return c;
}
static void fe_init (void)
{
  fe_is_initialized=TRUE;
  if ((!fe_use_fgets) && (isatty (STDIN_FILENO)))
  {
    /* Make sure stdin is a terminal. */
    char *term=getenv("TERM");

    /*setup echo*/
    if(isatty(STDOUT_FILENO))
    {
      fe_stdout_is_tty=1;
      fe_echo=stdout;
    }
    else
    {
      fe_stdout_is_tty=0;
      char *tty_name=ttyname(fileno(stdin));
      if (tty_name!=NULL)
        fe_echo = fopen( tty_name, "w" );
      else
        fe_echo = NULL;
      if (fe_echo==NULL)
      {
        fe_echo=stdout;
        printf("stdin is a tty, but ttyname fails\n");
        return;
      }
    }
    /* Save the terminal attributes so we can restore them later. */
    {
      struct termios tattr;
      tcgetattr (STDIN_FILENO, &fe_saved_attributes);
      #ifdef HAVE_FEREAD
        #ifdef HAVE_ATEXIT
          atexit(fe_reset_fe);
        #else
          on_exit(fe_reset_fe,NULL);
        #endif
      #endif

      /* Set the funny terminal modes. */
      tcgetattr (STDIN_FILENO, &tattr);
      tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
      tattr.c_cc[VMIN] = 1;
      tattr.c_cc[VTIME] = 0;
      tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
      /*ospeed=cfgetospeed(&tattr);*/
    }
    if(term==NULL)
    {
      printf("need TERM\n");
    }
    else if(tgetent(termcap_buff,term)<=0)
    {
      printf("could not access termcap data base\n");
    }
    else
    {
      #ifndef __CYGWIN__
      EXTERN_VAR char *BC;
      EXTERN_VAR char *UP;
      EXTERN_VAR char PC;
      #endif
      /* OB: why this ? HS: char t_buf[128] does not work with glibc2 systems */
      char *t_buf=(char *)omAlloc(128);
      /*char t_buf[128];*/
      char *temp;
      char** t_buf_ptr= &t_buf;
      /* Extract information that termcap functions use.  */
      temp = tgetstr ("pc", t_buf_ptr);
      PC = (temp!=NULL) ? *temp : '\0';
      BC=tgetstr("le",t_buf_ptr);
      UP=tgetstr("up",t_buf_ptr);

      /* Extract information we will use */
      colmax=tgetnum("co");
      pagelength=tgetnum("li");
      fe_cursor_line=pagelength-1;

      /* init screen */
      temp = tgetstr ("ti", t_buf_ptr);
      #if 0
      if (temp!=NULL) tputs(temp,1,fe_out_char);
      #endif

      /* printf("TERM=%s, co=%d, li=%d\n",term,colmax,pagelength);*/
    }

    fe_stdin_is_tty=1;
    fe_is_raw_tty=1;

    /* setup history */
    fe_hist=(char **)omAlloc0(fe_hist_max*sizeof(char *));
    omMarkAsStaticAddr(fe_hist);
    fe_hist_pos=0;
  }
  else
  {
    fe_stdin_is_tty=0;
    fe_echo=stdout;
  }
}

/* delete to end of line */
static void fe_ctrl_k(char *s,int i)
{
  int j=i;
  while(s[j]!='\0')
  {
    fputc(' ',fe_echo);
    j++;
  }
  while(j>i)
  {
    fputc('\b',fe_echo);
    j--;
  }
}

/* delete the line */
static void fe_ctrl_u(char *s,int *i)
{
  fe_ctrl_k(s,*i);
  while((*i)>0)
  {
    (*i)--;
    fputc('\b',fe_echo);
    fputc(' ',fe_echo);
    fputc('\b',fe_echo);
  }
}

/*2
* add s to the history
* if s is no the previous one, duplicate it
*/
static void fe_add_hist(char *s)
{
  if (s[0]!='\0') /* skip empty lines */
  {
    /* compare this line*/
    if (fe_hist_pos!=0)
    {
      if ((fe_hist[fe_hist_pos-1]!=NULL)
      && (strcmp(fe_hist[fe_hist_pos-1],s)==0))
        return;
    }
    else
    {
      if ((fe_hist[fe_hist_max-1]!=NULL)
      && (strcmp(fe_hist[fe_hist_max-1],s)==0))
        return;
    }
    /* normal case: enter a new line */
    /* first free the slot at position fe_hist_pos */
    if (fe_hist[fe_hist_pos]!=NULL)
    {
      omFree((ADDRESS)fe_hist[fe_hist_pos]);
    }
    /* and store a duplicate */
    fe_hist[fe_hist_pos]=omStrDup(s);
    omMarkAsStaticAddr(fe_hist[fe_hist_pos]);
    /* increment fe_hist_pos in a circular manner */
    fe_hist_pos++;
    if (fe_hist_pos==fe_hist_max) fe_hist_pos=0;
  }
}

static void fe_get_hist(char *s, int size, int *pos,int change, int incr)
{
  if (change)
    fe_add_hist(s);
  do
  {
    (*pos)+=incr;
    if((*pos)>=fe_hist_max) (*pos)-=fe_hist_max;
    else if((*pos)<0)       (*pos)+=fe_hist_max;
  }
  while (((*pos)!=0)&&(fe_hist[(*pos)]==NULL));
  memset(s,0,size);
  if (fe_hist[(*pos)]!=NULL)
  {
    strncpy(s,fe_hist[(*pos)],size-2);
  }
}

static int fe_getchar()
{
  char c='\0';
  while (1!=read (STDIN_FILENO, &c, 1));
  if (c == 033)
  {
    /* check for CSI */
    c='\0';
    while((-1 == read (STDIN_FILENO, &c, 1)) && (errno == EINTR));
    if (c == '[')
    {
      /* get command character */
      c='\0';
      while((-1 == read (STDIN_FILENO, &c, 1)) && (errno == EINTR));
      switch (c)
      {
        case 'D': /* left arrow key */
          c = feCTRL('B')/*002*/;
          break;
        case 'C': /* right arrow key */
          c = feCTRL('F')/*006*/;
          break;
        case 'A': /* up arrow key */
          c = feCTRL('P')/*020*/;
          break;
        case 'B': /* down arrow key */
          c = feCTRL('N')/*016*/;
          break;
      }
    }
  }
  return c;
}

static void fe_set_cursor(char *s,int i)
{
  char tgoto_buf[40];
  if (0)/*(fe_cursor_pos>1) && (i>0))*/
  {
    /*fputs(tgoto(tgetstr("cm",&tgoto_buf),fe_cursor_pos-1,fe_cursor_line),fe_echo);*/
    tputs(
      tgoto(tgetstr("cm",(char **)&tgoto_buf),fe_cursor_pos-1,fe_cursor_line),
      pagelength,fe_out_char);
    fputc(s[i-1],fe_echo);
  }
  else
  {
    /*fputs(
      tgoto(tgetstr("cm",&tgoto_buf),fe_cursor_pos,fe_cursor_line),fe_echo);*/
    tputs(tgoto(tgetstr("cm",(char **)&tgoto_buf),fe_cursor_pos,fe_cursor_line),
      pagelength,fe_out_char);
  }
  fflush(fe_echo);
}

char * fe_fgets_stdin_fe(char *pr,char *s, int size)
{
  if(!fe_is_initialized)
    fe_init();
  if (fe_stdin_is_tty)
  {
    int h=fe_hist_pos;
    int change=0;
    char c;
    int i=0;

    if (fe_is_raw_tty==0)
    {
      fe_temp_set();
    }

    fputs(pr,fe_echo); fflush(fe_echo);
    fe_cursor_pos=strlen(pr); /* prompt */

    memset(s,0,size);

    loop
    {
      c=fe_getchar();
      switch(c)
      {
        case feCTRL('M'):
        case feCTRL('J'):
        {
          fd_set fdset;
          struct timeval tv;
          int sel;

          fe_add_hist(s);
          i=strlen(s);
          if (i<size-1) s[i]='\n';
          fputc('\n',fe_echo);
          fflush(fe_echo);

          FD_ZERO (&fdset);
          FD_SET(STDIN_FILENO, &fdset);
          tv.tv_sec = 0;
          tv.tv_usec = 0;
          do
          {
            sel = select (STDIN_FILENO+1,
#ifdef hpux
                          (int *)fdset.fds_bits,
#else
                          &fdset,
#endif
                          NULL, NULL, &tv);
          } while( (sel == -1) && (errno == EINTR) );
          if (sel==0)
            fe_temp_reset();
          return s;
        }
        case feCTRL('H'):
        case 127:       /*delete the character left of the cursor*/
        {
          if (i==0) break;
          i--;
          fe_cursor_pos--;
          if(fe_cursor_pos<0)
          {
            fe_cursor_line--;
            fe_cursor_pos=colmax-1;
            fe_set_cursor(s,i);
          }
          else
          {
            fputc('\b',fe_echo);
          }
          /* NO BREAK : next: feCTRL('D') */
        }
        case feCTRL('D'):  /*delete the character under the cursor or eof*/
        {
          int j;
          if ((i==0) &&(s[0]=='\0')) return NULL; /*eof*/
          if (s[i]!='\0')
          {
            j=i;
            while(s[j]!='\0')
            {
              s[j]=s[j+1];
              fputc(s[j],fe_echo);
              j++;
            }
            fputc(' ',fe_echo);
            if (fe_cursor_pos+(j-i)>=colmax)
            {
              fe_set_cursor(s,i);
            }
            else
            {
              while(j>i)
              {
                fputc('\b',fe_echo);
                j--;
              }
            }
          }
          change=1;
          fflush(fe_echo);
          break;
        }
        case feCTRL('A'):  /* move the cursor to the beginning of the line */
        {
          if (i>=colmax-strlen(pr))
          {
            while (i>=colmax-strlen(pr))
            {
              i-=colmax;
              fe_cursor_line--;
            }
            i=0;
            fe_cursor_pos=strlen(pr);
            fe_set_cursor(s,i);
          }
          else
          {
            while(i>0)
            {
              i--;
              fputc('\b',fe_echo);
            }
            fe_cursor_pos=strlen(pr);
          }
          break;
        }
        case feCTRL('E'): /* move the cursor to the end of the line */
        {
          while(s[i]!='\0')
          {
            fputc(s[i],fe_echo);
            i++;
            fe_cursor_pos++;
            if(fe_cursor_pos>=colmax)
            {
              fe_cursor_pos=0;
              if(fe_cursor_line!=(pagelength-1))
                fe_cursor_line++;
            }
          }
          break;
        }
        case feCTRL('B'): /* move the cursor backward one character */
        {
          if (i>0)
          {
            i--;
            fputc('\b',fe_echo);
            fe_cursor_pos--;
            if(fe_cursor_pos<0)
            {
              fe_cursor_pos=colmax-1;
              fe_cursor_line--;
            }
          }
          break;
        }
        case feCTRL('F'): /* move the cursor forward  one character */
        {
          if(s[i]!='\0')
          {
            fputc(s[i],fe_echo);
            i++;
            fe_cursor_pos++;
            if(fe_cursor_pos>=colmax)
            {
              fe_cursor_pos=0;
              if(fe_cursor_line!=(pagelength-1))
                fe_cursor_line++;
            }
          }
          break;
        }
        case feCTRL('U'): /* delete entire input line */
        {
          fe_ctrl_u(s,&i);
          fe_cursor_pos=strlen(pr);
          memset(s,0,size);
          change=1;
          break;
        }
        #if 0
        case feCTRL('W'): /* test hist. */
        {
          int i;
          PrintS("\nstart hist\n");
          for(i=0;i<fe_hist_max;i++)
          {
            if(fe_hist[i]!=NULL)
            {
              Print("%2d ",i);
              if(i==fe_hist_pos) PrintS("-"); else PrintS(" ");
              if(i==h) PrintS(">"); else PrintS(" ");
              PrintS(fe_hist[i]);
              PrintLn();
            }
          }
          Print("end hist, next_pos=%d\n",fe_hist_pos);
          break;
        }
        #endif
        case feCTRL('K'): /* delete up to the end of the line */
        {
          fe_ctrl_k(s,i);
          memset(&(s[i]),'\0',size-i);
          /* s[i]='\0';*/
          change=1;
          break;
        }
        case feCTRL('L'): /* redraw screen */
        {
          char t_buf[40];
          char *t=t_buf;
          fe_cursor_line=i/colmax;
          /*fputs(tgetstr("cl",&t),fe_echo);*/
          tputs(tgetstr("cl",&t),pagelength,fe_out_char);
          fflush(fe_echo);
          fputs(pr,fe_echo);
          fputs(s,fe_echo);
          fe_set_cursor(s,i);
          break;
        }
        case feCTRL('P'): /* previous line */
        {
          fe_ctrl_u(s,&i);
          fe_get_hist(s,size,&h,change,-1);
          while(s[i]!='\0')
          {
            fputc(s[i],fe_echo);
            i++;
          }
          fe_cursor_pos=strlen(pr)+i/*strlen(s)*/;
          change=0;
          break;
        }
        case feCTRL('N'): /* next line */
        {
          fe_ctrl_u(s,&i);
          fe_get_hist(s,size,&h,change,1);
          while(s[i]!='\0')
          {
            fputc(s[i],fe_echo);
            i++;
          }
          fe_cursor_pos=strlen(pr)+i/*strlen(s)*/;
          change=0;
          break;
        }
        default:
        {
          if ((c>=' ')&&(c<=126))
          {
            fputc (c,fe_echo);
            fe_cursor_pos++;
            if(fe_cursor_pos>=colmax)
            {
              fe_cursor_pos=0;
              if(fe_cursor_line!=(pagelength-1))
                fe_cursor_line++;
            }
            if (s[i]!='\0')
            {
              /* shift by 1 to the right */
              int j=i;
              int l;
              while ((s[j]!='\0')&&(j<size-2)) j++;
              l=j-i;
              while (j>i) { s[j]=s[j-1]; j--; }
              /* display */
              fwrite(s+i+1,l,1,fe_echo);
              fflush(fe_echo);
              /* set cursor */
              if(fe_cursor_pos+l>=colmax)
              {
                while(fe_cursor_pos+l>=colmax)
                {
                  fe_cursor_line--;
                  l-=colmax;
                }
                fe_set_cursor(s,i);
              }
              else
              {
                while(l>0)
                {
                  l--;
                  fputc('\b',fe_echo);
                }
              }
              fflush(fe_echo);
            }
            if (i<size-1) s[i]=c;
            i++;
            change=1;
          }
        }
      } /* switch */
      fflush(fe_echo);
    } /* loop */
  }
  /*else*/
    return fgets(s,size,stdin);
}

//int main (void)
//{
//  char b[200];
//  char * m_eof;
//
//  fe_init();
//  while(1)
//  {
//    m_eof=fe_fgets_stdin_fe("> ",b,200);
//    if (!m_eof) break;
//    printf(">>%s<<\n",b);
//  }
//
//  return 0;
//}
#endif

/* ================================================================ */
#if defined(HAVE_DYN_RL)
#include <unistd.h>
#include "kernel/mod_raw.h"

  typedef char **CPPFunction ();

  char *(*fe_filename_completion_function)(); /* 3 */
  char *(* fe_readline) ();                   /* 4 */
  VAR void (*fe_add_history) ();                  /* 5 */
  VAR char ** fe_rl_readline_name;                /* 6 */
  VAR char **fe_rl_line_buffer;                   /* 7 */
  char **(*fe_completion_matches)();          /* 8 */
  VAR CPPFunction **fe_rl_attempted_completion_function; /* 9 */
  VAR FILE ** fe_rl_outstream;                    /* 10 */
  VAR int (*fe_write_history) ();                 /* 11 */
  VAR int (*fe_history_total_bytes) ();           /* 12 */
  VAR void (*fe_using_history) ();                /* 13 */
  VAR int (*fe_read_history) ();                  /* 14 */

VAR void * fe_rl_hdl=NULL;

char *command_generator (char *text, int state);

/* Attempt to complete on the contents of TEXT.  START and END show the
*   region of TEXT that contains the word to complete.  We can use the
*   entire line in case we want to do some simple parsing.  Return the
*   array of matches, or NULL if there aren't any.
*/
char ** singular_completion (char *text, int start, int end)
{
  /* If this word is not in a string, then it may be a command
     to complete.  Otherwise it may be the name of a file in the current
     directory. */
  char **m;
  if ((*fe_rl_line_buffer)[start-1]=='"')
    return (*fe_completion_matches) (text, *fe_filename_completion_function);
  m=(*fe_completion_matches) (text, command_generator);
  if (m==NULL)
  {
    m=(char **)malloc(2*sizeof(char*));
    m[0]=(char *)malloc(end-start+2);
    strncpy(m[0],text,end-start+1);
    m[1]=NULL;
  }
  return m;
}


int fe_init_dyn_rl()
{
  int res=0;
  loop
  {
    fe_rl_hdl=dynl_open("libreadline.so");
    if (fe_rl_hdl==NULL) fe_rl_hdl=dynl_open("libreadline.so.2");
    if (fe_rl_hdl==NULL) fe_rl_hdl=dynl_open("libreadline.so.3");
    if (fe_rl_hdl==NULL) fe_rl_hdl=dynl_open("libreadline.so.4");
    if (fe_rl_hdl==NULL) fe_rl_hdl=dynl_open("libreadline.so.5");
    if (fe_rl_hdl==NULL) fe_rl_hdl=dynl_open("libreadline.so.6");
    if (fe_rl_hdl==NULL) fe_rl_hdl=dynl_open("libreadline.so.7");
    if (fe_rl_hdl==NULL) fe_rl_hdl=dynl_open("libreadline.so.8");
    if (fe_rl_hdl==NULL) fe_rl_hdl=dynl_open("libreadline.so.9");
    if (fe_rl_hdl==NULL) { return 1;}

    fe_filename_completion_function=
      dynl_sym(fe_rl_hdl, "filename_completion_function");
    if (fe_filename_completion_function==NULL) { res=3; break; }
    fe_readline=dynl_sym(fe_rl_hdl,"readline");
    if (fe_readline==NULL) { res=4; break; }
    fe_add_history=dynl_sym(fe_rl_hdl,"add_history");
    if (fe_add_history==NULL) { res=5; break; }
    fe_rl_readline_name=(char**)dynl_sym(fe_rl_hdl,"rl_readline_name");
    if (fe_rl_readline_name==NULL) { res=6; break; }
    fe_rl_line_buffer=(char**)dynl_sym(fe_rl_hdl,"rl_line_buffer");
    if (fe_rl_line_buffer==NULL) { res=7; break; }
    fe_completion_matches=dynl_sym(fe_rl_hdl,"completion_matches");
    if (fe_completion_matches==NULL) { res=8; break; }
    fe_rl_attempted_completion_function=
      dynl_sym(fe_rl_hdl,"rl_attempted_completion_function");
    if (fe_rl_attempted_completion_function==NULL) { res=9; break; }
    fe_rl_outstream=(FILE**)dynl_sym(fe_rl_hdl,"rl_outstream");
    if (fe_rl_outstream==NULL) { res=10; break; }
    fe_write_history=dynl_sym(fe_rl_hdl,"write_history");
    if (fe_write_history==NULL) { res=11; break; }
    fe_history_total_bytes=dynl_sym(fe_rl_hdl,"history_total_bytes");
    if (fe_history_total_bytes==NULL) { res=12; break; }
    fe_using_history=dynl_sym(fe_rl_hdl,"using_history");
    if (fe_using_history==NULL) { res=13; break; }
    fe_read_history=dynl_sym(fe_rl_hdl,"read_history");
    if (fe_read_history==NULL) { res=14; break; }
    break;
  }
  using_history_called=FALSE;
  if (res!=0) dynl_close(fe_rl_hdl);
  else
  {
    char *p;
    /* more init stuff: */
    /* Allow conditional parsing of the ~/.inputrc file. */
    (*fe_rl_readline_name) = "Singular";
    /* Tell the completer that we want a crack first. */
    (*fe_rl_attempted_completion_function) = (CPPFunction *)singular_completion;
    /* try to read a history */
    using_history_called=TRUE;
    (*fe_using_history)();
    p = getenv("SINGULARHIST");
    if (p==NULL) p=SINGULARHIST_FILE;
    if (strlen(p) != 0)
    {
      (*fe_read_history) (p);
    }
  }
  return res;
}
#endif

/* ===================================================================*/
/* =          fe_reset_input_mode (all possibilities)               = */
/* ===================================================================*/
#if defined(HAVE_READLINE) && !defined(HAVE_FEREAD) && !defined(HAVE_DYN_RL)
extern int history_total_bytes();
extern int write_history (const char *);
#endif
void fe_reset_input_mode ()
{
#if defined(HAVE_DYN_RL)
  char *p = getenv("SINGULARHIST");
  if (p==NULL) p=SINGULARHIST_FILE;
  if ((strlen(p) != 0) && (fe_history_total_bytes != NULL))
  {
    if((using_history_called && (*fe_history_total_bytes)()!=0))
      (*fe_write_history) (p);
  }
#elif defined(HAVE_READLINE) && !defined(HAVE_FEREAD) && !defined(HAVE_DYN_RL)
  char *p = getenv("SINGULARHIST");
  if (p==NULL) p=SINGULARHIST_FILE;
  if (strlen(p) != 0)
  {
    if(using_history_called &&(history_total_bytes()!=0))
      write_history (p);
  }
#elif defined(HAVE_FEREAD)
  #ifndef HAVE_ATEXIT
  fe_reset_fe(NULL,NULL);
  #else
  fe_reset_fe();
  #endif
#endif
}

