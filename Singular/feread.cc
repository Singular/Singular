/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: feread.cc,v 1.11 1998-05-15 16:03:49 Singular Exp $ */
/*
* ABSTRACT: input from ttys, simulating fgets
*/


#include "mod2.h"

#ifdef HAVE_FEREAD
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef MSDOS
#include <pc.h>
#else
#ifdef atarist
#include <ioctl.h>
#else
#ifdef NeXT
#include <sgtty.h>
#include <sys/ioctl.h>
#else
#include <termios.h>
#endif
#endif
#endif
#include "tok.h"
#include "mmemory.h"
#include "febase.h"

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
static FILE *  fe_echo; /*the output file for echoed characters*/

#define fe_hist_max 32
char ** fe_hist=NULL;
int     fe_hist_pos;

#ifndef MSDOS
  #ifdef HAVE_ATEXIT
    void fe_reset_input_mode (void)
  #else
    extern "C" int on_exit(void (*f)(int, void *), void *arg);

    void fe_reset_input_mode (int i, void *v)
  #endif
  {
    if (fe_stdin_is_tty)
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
      fe_stdin_is_tty=0;
      int i;
      for(i=fe_hist_max-1;i>=0;i--)
      {
        FreeL((ADDRESS)fe_hist[i]);
      }
      Free((ADDRESS)fe_hist,fe_hist_max*sizeof(char *));
      fe_hist=NULL;
      if (!fe_stdout_is_tty)
      {
        fclose(fe_echo);
      }
    }
  }
  void fe_temp_reset (void)
  {
    if (fe_stdin_is_tty)
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
    }
  }
  void fe_temp_set (void)
  {
    if(fe_stdin_is_tty)
    {
      #ifdef atarist
        /*set line wrap mode*/
        if(isatty(STDOUT_FILENO))
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
    }
  }
#endif

void fe_set_input_mode (void)
{
  #ifdef atarist
    /*set line wrap mode*/
    if(isatty(STDOUT_FILENO))
    {
      printf("\033v");
    }
  #endif
  /* Make sure stdin is a terminal. */
  if ((!fe_use_fgets) && (isatty (STDIN_FILENO)))
  {
    #ifndef MSDOS
      #if defined( atarist ) || defined( NeXT )
        struct sgttyb tattr;
        #ifdef atarist
          gtty(0, &fe_saved_attributes);
        #else
          ioctl(STDIN_FILENO, TIOCGETP, &fe_saved_attributes);
        #endif
      #else
        struct termios tattr;

        /* Save the terminal attributes so we can restore them later. */
        tcgetattr (STDIN_FILENO, &fe_saved_attributes);
      #endif
      #ifdef HAVE_ATEXIT
        atexit(fe_reset_input_mode);
      #else
        on_exit(fe_reset_input_mode,NULL);
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
    #endif

    fe_stdin_is_tty=1;

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
        fe_echo = myfopen( "/dev/tty", "w" );
      #else
        fe_echo = myfopen( ttyname(fileno(stdin)), "w" );
      #endif
    }

    /* setup history */
    fe_hist=(char **)Alloc0(fe_hist_max*sizeof(char *));
    fe_hist_pos=0;
  }
  else
  {
    fe_stdin_is_tty=0;
    fe_echo=stdout;
  }
}

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

static void fe_ctrl_x(char *s,int &i)
{
  fe_ctrl_k(s,i);
  while(i>0)
  {
    i--;
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
      FreeL((ADDRESS)fe_hist[fe_hist_pos]);
    }
    /* and store a duplicate */
    fe_hist[fe_hist_pos]=mstrdup(s);
    /* increment fe_hist_pos in a circular manner */
    fe_hist_pos++;
    if (fe_hist_pos==fe_hist_max) fe_hist_pos=0;
  }
}

static void fe_get_hist(char *s, int size, int &pos,int change, int incr)
{
  if (change)
    fe_add_hist(s);
  do
  {
    pos+=incr;
    if(pos>=fe_hist_max) pos-=fe_hist_max;
    else if(pos<0)       pos+=fe_hist_max;
  }
  while ((pos!=0)&&(fe_hist[pos]==NULL));
  memset(s,0,size);
  if (fe_hist[pos]!=NULL)
  {
    strncpy(s,fe_hist[pos],size-2);
  }
}

char * fe_fgets_stdin(char *s, int size)
{
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
    memset(s,0,size);

    loop
    {
      #ifndef MSDOS
        c=0;
        read (STDIN_FILENO, &c, 1);
      #else
        c=getkey();
      #endif
      #ifndef MSDOS
      if (c == 033)
      {
        /* check for CSI */
        c=0;
        read (STDIN_FILENO, &c, 1);
        if (c == '[')
        {
          /* get command character */
          c=0;
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
      switch(c)
      {
        case feCTRL('M'):
        case feCTRL('J'):
        {
          fe_add_hist(s);
          i=strlen(s);
          if (i<size-1) s[i]='\n';
          fputc('\n',fe_echo);
          fflush(fe_echo);
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
          fputc('\b',fe_echo);
          fputc(' ',fe_echo);
          fputc('\b',fe_echo);
          fflush(fe_echo);
          change=1;
        }
        case feCTRL('D'):  /*delete the character under the cursor or eof*/
        {
          int j;
          if ((i==0)&&(c==feCTRL('D')&&(s[0]=='\0'))) return NULL; /*eof*/
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
            while(j>i)
            {
              fputc('\b',fe_echo);
              j--;
            }
          }
          #ifdef MSDOS
            fputc('\b',fe_echo);
          #endif
          change=1;
          break;
        }
        case feCTRL('A'):  /* move the cursor to the beginning of the line */
        {
          while(i>0)
          {
            i--;
            fputc('\b',fe_echo);
          }
          break;
        }
        case feCTRL('E'): /* move the cursor to the end of the line */
        {
          while(s[i]!='\0')
          {
            fputc(s[i],fe_echo);
            i++;
          }
          break;
        }
        case feCTRL('B'): /* move the cursor backward one character */
        {
          i--;
          fputc('\b',fe_echo);
          break;
        }
        case feCTRL('F'): /* move the cursor forward  one character */
        {
          if(s[i]!='\0')
          {
            fputc(s[i],fe_echo);
            i++;
          }
          break;
        }
        /* change to ^U, to be consistent with readline:*/
        case feCTRL('U'): /* delete entire input line */
        {
          fe_ctrl_x(s,i);
          memset(s,0,size);
          change=1;
          break;
        }
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
        case feCTRL('K'): /* delete up to the end of the line */
        {
          fe_ctrl_k(s,i);
          s[i]='\0';
          change=1;
          break;
        }
        case feCTRL('P'): /* previous line */
        {
          fe_ctrl_x(s,i);
          fe_get_hist(s,size,h,change,-1);
          while(s[i]!='\0')
          {
            fputc(s[i],fe_echo);
            i++;
          }
          change=0;
          break;
        }
        case feCTRL('N'): /* next line */
        {
          fe_ctrl_x(s,i);
          fe_get_hist(s,size,h,change,1);
          while(s[i]!='\0')
          {
            fputc(s[i],fe_echo);
            i++;
          }
          change=0;
          break;
        }
        default:
        {
          if ((c>=' ')&&(c<=126))
          {
            fputc (c,fe_echo);
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
              /* set cursor */
              while(l>0)
              {
                l--;
                fputc('\b',fe_echo);
              }
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
//  fe_set_input_mode();
//  while(1)
//  {
//    m_eof=fe_fgets_stdin(b,200);
//    if (!m_eof) break;
//    printf(">>%s<<\n",b);
//  }
//
//  return 0;
//}
#endif
/*=======================================================================*/
#if defined(HAVE_READLINE) && !defined(HAVE_FEREAD) 

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/errno.h>

extern "C" {
#include <readline/readline.h>
#include <readline/history.h>
}

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

#include "febase.h"
#include "ipshell.h"

BOOLEAN fe_use_fgets=FALSE;

/* Tell the GNU Readline library how to complete.  We want to try to complete
   on command names  or on filenames if it is preceded by " */

/* Generator function for command completion.  STATE lets us know whether
*   to start from scratch; without any state (i.e. STATE == 0), then we
*   start at the top of the list.
*/
char *command_generator (char *text, int state)
{
  static int list_index, len;
  char *name;

  /* If this is a new word to complete, initialize now.  This includes
     saving the length of TEXT for efficiency, and initializing the index
     variable to 0. */
  if (state==0)
  {
    list_index = 1;
    len = strlen (text);
  }

  /* Return the next name which partially matches from the command list. */
  while ((name = cmds[list_index].name)!=NULL)
  {
    list_index++;

    if (strncmp (name, text, len) == 0)
      return (strdup(name));
  }

  /* If no names matched, then return NULL. */
  return ((char *)NULL);
}

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
  if (rl_line_buffer[start-1]=='"')
    return completion_matches (text, filename_completion_function);
  char **m=completion_matches (text, command_generator);
  if (m==NULL)
  {
    m=(char **)malloc(2*sizeof(char*));
    m[0]=(char *)malloc(end-start+2);
    strncpy(m[0],text,end-start+1);
    m[1]=NULL;
  }
  return m;
}

void fe_set_input_mode(void)
{
  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = "Singular";

  if(!fe_use_fgets)
  {
    /* Tell the completer that we want a crack first. */
    rl_attempted_completion_function = (CPPFunction *)singular_completion;

    /* set the output stream */
    if(!isatty(STDOUT_FILENO))
    {
      #ifdef atarist
        rl_outstream = myfopen( "/dev/tty", "w" );
      #else
        rl_outstream = myfopen( ttyname(fileno(stdin)), "w" );
      #endif
    }

    /* try to read a history */
    using_history();
    read_history (".singular_hist");
  }  
}

void fe_reset_input_mode (void)
{
  /* try to read a history */
  if(!feBatch && !fe_use_fgets && (history_total_bytes()!=0))
    write_history (".singular_hist");
}

char * fe_fgets_stdin_rl(char *pr,char *s, int size)
{
  if (feBatch)
    return NULL;
  if(fe_use_fgets)
  {
    PrintS(pr);mflush();
    return fgets(s,size,stdin);
  }

  char *line;

  line = readline (pr);

  if (line==NULL)
    return NULL;

  if (*line!='\0')
  {
    add_history (line);
  }
  int l=strlen(line);
  if (l>=size-1)
  {
    strncpy(s,line,size);
  }
  else
  {
    strncpy(s,line,l);
    s[l]='\n';
    s[l+1]='\0';
  }
  free (line);

  return s;
}
#endif
