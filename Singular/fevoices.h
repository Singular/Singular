#ifndef FEVOICES_H
#define FEVOICES_H
/*!
!
***************************************
 * *  Computer Algebra System SINGULAR     *
 * ****************************************/
/*!
!

 * * ABSTRACT: class Voice
 * 

*/

#include "reporter/reporter.h"

#include "resources/feResource.h"

#include "kernel/structs.h"

enum   feBufferTypes
{
  BT_none  = 0,  // entry level
  BT_break = 1,  // while, for
  BT_proc,       // proc
  BT_example,    // example
  BT_file,       // <"file"
  BT_execute,    // execute
  BT_if,         // if
  BT_else        // else
};

enum   feBufferInputs
{
  BI_stdin = 1,
  BI_buffer,
  BI_file
};
enum noeof_t
{
  noeof_brace = 1,
  noeof_asstring,
  noeof_block,
  noeof_bracket,
  noeof_comment,
  noeof_procname,
  noeof_string
};  /*!
!
 for scanner.l 

*/

extern int yylineno;
extern char my_yylinebuf[80];
extern const char sNoName_fe[];

#ifdef __cplusplus

/*!
!
 the C++-part: 

*/

// LANG_TOP     : Toplevel package only
// LANG_SINGULAR:
// LANG_C       :
//

class Voice
{
  public:
    Voice  * next;
    Voice  * prev;
    char   * filename;    // file name or proc name
    procinfo * pi;        // proc info
    void   * oldb;        // internal scanner buffer
    // for files only:
    FILE * files;         // file handle
    // for buffers only:
    char * buffer;        // buffer pointer
    long   fptr;          // current position in buffer
    long   ftellptr;      // with glibc 2.22, file position gets lost
                          // in sig_chld_hdl (ssi:ffork link), see examples/waitall.sing

    int    start_lineno;  // lineno, to restore in recursion
    int    curr_lineno;   // current lineno
    feBufferInputs   sw;  // BI_stdin: read from STDIN
                          // BI_buffer: buffer
                          // BI_file: files
    char   ifsw;          // if-switch:
            /*!
!
1 ifsw==0: no if statement, else is invalid
            *       ==1: if (0) processed, execute else
            *       ==2: if (1) processed, else allowed but not executed
            

*/
    feBufferTypes   typ;  // buffer type: see BT_..

  Voice() { memset(this,0,sizeof(*this));}
  feBufferTypes Typ();
  void Next();
} ;

extern Voice  *currentVoice;

Voice * feInitStdin(Voice *pp);

const  char * VoiceName();
void    VoiceBackTrack();
BOOLEAN contBuffer(feBufferTypes typ);
BOOLEAN exitBuffer(feBufferTypes typ);
BOOLEAN exitVoice();
void    monitor(void *F, int mode); /*!
!
 FILE*, int 

*/
BOOLEAN newFile(char* fname);
void    newBuffer(char* s, feBufferTypes t, procinfo *pname = NULL, int start_lineno = 0);
void *  myynewbuffer();
void    myyoldbuffer(void * oldb);

#endif
#endif

