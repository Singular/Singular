/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: fereadl.c,v 1.22 2002-07-24 15:36:56 Singular Exp $ */
/*
* ABSTRACT: input from ttys, simulating fgets
*/


#include "mod2.h"
#include "tok.h"
#include "febase.h"
#include "omalloc.h"
#include "structs.h"
#include "febase.h"

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

    #ifdef atarist
      #include <ioctl.h>
    #else
      #ifdef NeXT
        #include <sgtty.h>
        #include <sys/ioctl.h>
      #endif
    #endif
  #endif


#ifndef STDIN_FILENO
  #define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
  #define STDOUT_FILENO 1
#endif

#define feCTRL(C) ((C) & 0x1F)    /* <ctrl> character  */

#ifndef MSDOS

  /* Use this variable to remember original terminal attributes. */
  #if defined( atarist ) || defined( NeXT )
    struct sgttyb  fe_saved_attributes;
  #else
    struct termios fe_saved_attributes;
  #endif
#endif

static BOOLEAN fe_stdout_is_tty;
static BOOLEAN fe_stdin_is_tty;
BOOLEAN fe_use_fgets=FALSE;
static BOOLEAN fe_is_initialized=FALSE;
FILE *  fe_echo; /*the output file for echoed characters*/

#define fe_hist_max 32
char ** fe_hist=NULL;
short   fe_hist_pos;
BOOLEAN fe_is_raw_tty=0;
int     fe_cursor_pos; /* 0..colmax-1*/
int     fe_cursor_line; /* 0..pagelength-1*/

#ifndef MSDOS
  #ifndef HAVE_ATEXIT
    int on_exit(void (*f)(int, void *), void *arg);
    void fe_reset_fe (int i, void *v)
  #else
    void fe_reset_fe (void)
  #endif
  {
    if (fe_stdin_is_tty)
    {
      int i;
      if (fe_is_raw_tty)
      {
        #ifdef atarist
          stty(0, &fe_saved_attributes);
        #else
          #ifdef NeXT
            ioctl(STDIN_FILENO, TIOCSETP, &fe_saved_attributes);
          #else
            tcsetattr (STDIN_FILENO, TCSANOW, &fe_saved_attributes);
          #endif
        #endif
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
      #ifdef atarist
        stty(0, &fe_saved_attributes);
      #else
        #ifdef NeXT
          ioctl(STDIN_FILENO, TIOCSETP, &fe_saved_attributes);
        #else
          tcsetattr (STDIN_FILENO, TCSANOW, &fe_saved_attributes);
        #endif
      #endif
      fe_is_raw_tty=0;
    }
  }
  void fe_temp_set (void)
  {
    if(fe_is_raw_tty==0)
    {
      #ifdef atarist
        /*set line wrap mode*/
        if(fe_stdout_is_tty)
        {
          printf("\033v");
        }
      #endif
      #if defined( atarist ) || defined( NeXT )
        struct sgttyb tattr;
      #else
        struct termios tattr;
      #endif

      /* Set the funny terminal modes. */
      #ifdef atarist
         gtty(0, &tattr);
         tattr.sg_flags |= RAW;
         tattr.sg_flags |= CBREAK;
         tattr.sg_flags &= ~ECHO;
         stty(0, &tattr);
      #else
        #ifdef NeXT
          ioctl(STDIN_FILENO, TIOCGETP, &tattr);
          //tattr.sg_flags |= RAW;
          tattr.sg_flags |= CBREAK;
          tattr.sg_flags &= ~ECHO;
          ioctl(STDIN_FILENO, TIOCSETP, &tattr);
          ioctl(STDOUT_FILENO, TIOCGETP, &tattr);
          tattr.sg_flags |= CRMOD;
          ioctl(STDOUT_FILENO, TIOCSETP, &tattr);
        #else
          tcgetattr (STDIN_FILENO, &tattr);
          tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
          tattr.c_cc[VMIN] = 1;
          tattr.c_cc[VTIME] = 0;
          tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
        #endif
      #endif
      fe_is_raw_tty=1;
    }
  }
#endif

static char termcap_buff[2048];
static int fe_out_char(int c)
{
  fputc(c,fe_echo);
  return c;
}
void fe_init (void)
{
  #ifdef MSDOS
  /*extern short ospeed;*/
  #endif
  fe_is_initialized=TRUE;
  if ((!fe_use_fgets) && (isatty (STDIN_FILENO)))
  {
    /* Make sure stdin is a terminal. */
    #ifndef MSDOS
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
        #ifdef atarist
          fe_echo = fopen( "/dev/tty", "w" );
        #else
          fe_echo = fopen( ttyname(fileno(stdin)), "w" );
        #endif
      }
      /* Save the terminal attributes so we can restore them later. */
      {
        #if defined( atarist ) || defined( NeXT )
          struct sgttyb tattr;
          #ifdef atarist
            gtty(0, &fe_saved_attributes);
          #else
            ioctl(STDIN_FILENO, TIOCGETP, &fe_saved_attributes);
          #endif
        #else
          struct termios tattr;
          tcgetattr (STDIN_FILENO, &fe_saved_attributes);
        #endif
        #ifdef HAVE_ATEXIT
          atexit(fe_reset_fe);
        #else
          on_exit(fe_reset_fe,NULL);
        #endif

      /* Set the funny terminal modes. */
        #ifdef atarist
          gtty(0, &tattr);
          tattr.sg_flags |= RAW;
          tattr.sg_flags |= CBREAK;
          tattr.sg_flags &= ~ECHO;
          stty(0, &tattr);
        #else
          #ifdef NeXT
            ioctl(STDIN_FILENO, TIOCGETP, &tattr);
            //tattr.sg_flags |= RAW;
            tattr.sg_flags |= CBREAK;
            tattr.sg_flags &= ~ECHO;
            ioctl(STDIN_FILENO, TIOCSETP, &tattr);
            ioctl(STDOUT_FILENO, TIOCGETP, &tattr);
            tattr.sg_flags |= CRMOD;
            ioctl(STDOUT_FILENO, TIOCSETP, &tattr);
          #else
            tcgetattr (STDIN_FILENO, &tattr);
            tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
            tattr.c_cc[VMIN] = 1;
            tattr.c_cc[VTIME] = 0;
            tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
          #endif
          /*ospeed=cfgetospeed(&tattr);*/
        #endif
      }
    #endif
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
      #ifndef ix86_Win
      extern char *BC;
      extern char *UP;
      extern char PC;
      #endif
      /* OB: why this ??? */
      /* char *t_buf=(char *)omAlloc(128); */
      char t_buf[128];
      char *temp;

      /* Extract information that termcap functions use.  */
      temp = tgetstr ("pc", (char **)&t_buf);
      PC = (temp!=NULL) ? *temp : '\0';
      BC=tgetstr("le",(char **)&t_buf);
      UP=tgetstr("up",(char **)&t_buf);

      /* Extract information we will use */
      colmax=tgetnum("co");
      pagelength=tgetnum("li");
      fe_cursor_line=pagelength-1;

      /* init screen */
      temp = tgetstr ("ti", (char **)&t_buf);
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
  #ifndef MSDOS
  char c='\0';
  while (1!=read (STDIN_FILENO, &c, 1));
  #else
  int c=getkey();
  #endif
  #ifndef MSDOS
  if (c == 033)
  {
    /* check for CSI */
    c='\0';
    read (STDIN_FILENO, &c, 1);
    if (c == '[')
    {
      /* get command character */
      c='\0';
      read (STDIN_FILENO, &c, 1);
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
  #endif
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
    #ifdef MSDOS
      int c;
    #else
      char c;
    #endif
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
          #ifdef hpux
            sel = select (STDIN_FILENO+1, (int *)fdset.fds_bits, NULL, NULL, &tv);
          #else
            sel = select (STDIN_FILENO+1, &fdset, NULL, NULL, &tv);
          #endif
          if (sel==0)
            fe_temp_reset();
          return s;
        }
        #ifdef MSDOS
        case 0x153:
        #endif
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
          #ifdef MSDOS
          fputc('\b',fe_echo);
          #endif
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
