%{
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#include "kernel/mod2.h"
#include "omalloc/omalloc.h"
#include "Singular/tok.h"
#include "Singular/stype.h"
#include "Singular/ipshell.h"
#include "Singular/ipid.h"
#include "Singular/fevoices.h"
#include "kernel/oswrapper/feread.h"

int feReadLine(char* b, int l);
#define ALLOC(a) omAlloc((a))
#ifndef NEW_FLEX
#endif /* NEW_LEX */
int blocknest = 0;
extern char * yytext;
//extern unsigned char * yytext;
extern int yyleng;
extern int inerror;

#if defined(__GNUC__) || defined(__clang__)
#define YY_LAMBDA_OUTLINE __attribute__((section(".lambda_text"),noinline))
#define YY_LAMBDA_COLD YY_LAMBDA_OUTLINE
#define YY_LAMBDA_NOINLINE YY_LAMBDA_OUTLINE
#if defined(__GNUC__) && !defined(__clang__)
#define YY_LAMBDA_NO_PATTERN \
  __attribute__((optimize("no-tree-loop-distribute-patterns")))
#else
#define YY_LAMBDA_NO_PATTERN
#endif
#else
#define YY_LAMBDA_OUTLINE
#define YY_LAMBDA_COLD
#define YY_LAMBDA_NOINLINE
#define YY_LAMBDA_NO_PATTERN
#endif

enum yyLambdaProbeResult
{
  yyLambdaProbeNoMatch,
  yyLambdaProbeMatch,
  yyLambdaProbeUnavailable
};

/* First classify a possible anonymous-procedure header without advancing a
   voice or changing diagnostics.  Only a confirmed header is then consumed
   through flex's buffer machinery. */
static yyLambdaProbeResult yyLambdaProbe() YY_LAMBDA_OUTLINE;
static int yyLambdaParamToken(YYSTYPE *lvalp) YY_LAMBDA_COLD;
static int yyLambdaOpeningParenToken(YYSTYPE *lvalp) YY_LAMBDA_NOINLINE;
static int yyLambdaInput() YY_LAMBDA_OUTLINE;
static void yyLambdaRewind() YY_LAMBDA_OUTLINE;
static void yyLambdaLeaveOpeningBrace() YY_LAMBDA_OUTLINE;
static void yyLambdaCaptureBegin() YY_LAMBDA_OUTLINE;
static int yyLambdaCaptureInput() YY_LAMBDA_OUTLINE;
static void yyLambdaCaptureDiscard() YY_LAMBDA_OUTLINE;
static void yyLambdaCaptureReplay() YY_LAMBDA_OUTLINE;
static int yyLambdaTrackedInput() YY_LAMBDA_COLD;
static void yyLambdaSkipBlockComment() YY_LAMBDA_COLD;
static void yyLambdaErrorUserAction() YY_LAMBDA_COLD;
static void yyLambdaReplayDiscardAll() YY_LAMBDA_OUTLINE;
static const char *yyLambdaFindByte(const char *pos, const char *end,
                                    char needle)
  YY_LAMBDA_OUTLINE YY_LAMBDA_NO_PATTERN;

struct yyLambdaReplayState;
static yyLambdaReplayState *yyLambdaReplays=NULL;

enum yyLambdaArrowState
{
  yyLambdaArrowUnknown=0,
  yyLambdaArrowNo,
  yyLambdaArrowMaybe
};

#if defined(__GNUC__) || defined(__clang__)
__attribute__((visibility("hidden")))
#endif
char *yyLambdaPendingParams=NULL;

#if defined(__GNUC__) || defined(__clang__)
__attribute__((visibility("hidden")))
#endif
void yyLambdaReplayLine(long offset) YY_LAMBDA_COLD;

#if defined(__GNUC__) || defined(__clang__)
__attribute__((visibility("hidden")))
#endif
void yyLambdaReplayFinishInput() YY_LAMBDA_COLD;

/* Preserve the historical one-token return for ordinary parentheses.  Cache
   the arrow classification in the voice itself, so entering and leaving a
   procedure needs no global-cache maintenance. */
#define YY_LAMBDA_FAST_ORDINARY_PAREN() \
  do { \
    if (LIKELY((currentVoice!=NULL) && \
               (currentVoice->lambda_arrow_state==yyLambdaArrowNo))) \
      return '('; \
    if (LIKELY(yy_hold_char=='(')) \
      return '('; \
    if (yytext_ptr>yy_current_buffer->yy_ch_buf) \
    { \
      const unsigned int previous=(unsigned char)yytext_ptr[-1]; \
      const unsigned int folded=previous | 0x20U; \
      if (LIKELY(((folded-(unsigned int)'a')<=25U) || \
                 ((previous-(unsigned int)'0')<=9U))) \
        return '('; \
    } \
  } while (0)

struct yyLambdaArrowScanState
{
  unsigned char stage;
  unsigned char continuation_state;
};

/* Limit speculative whole-source scans.  Small scripts and repeatedly parsed
   loop bodies benefit from an exact answer; large sources retain the regular
   local checks instead of paying an unbounded first-parenthesis cost. */
static const long yyLambdaFileArrowScanLimit=8L*1024L*1024L;
static const size_t yyLambdaBufferArrowScanLimit=1024UL*1024UL;

static inline BOOLEAN
yyLambdaArrowScanBytes(yyLambdaArrowScanState *state,
                       const unsigned char *bytes, const size_t count)
{
  for (size_t i=0;i<count;i++)
  {
    const int c=bytes[i];

    /* feReadLine removes backslash-newline continuations before flex sees
       file input.  Apply the same filter to this raw whole-source scan,
       including when the sequence crosses an fread() boundary. */
    if (state->continuation_state==1)
    {
      state->continuation_state=0;
      if (c=='\n') continue;
      if (c=='\r')
      {
        state->continuation_state=2;
        continue;
      }
      state->stage=0;
    }
    else if (state->continuation_state==2)
    {
      state->continuation_state=0;
      if (c=='\n') continue;
      state->stage=0;
    }
    if (c=='\\')
    {
      state->continuation_state=1;
      continue;
    }

    switch (state->stage)
    {
      case 0:
        if (c==')') state->stage=1;
        break;
      case 1:
        if ((c==' ') || (c=='\t') || (c=='\r') || (c=='\n')) break;
        if (c=='-') state->stage=2;
        else state->stage=(c==')') ? 1 : 0;
        break;
      case 2:
        if (c=='>') state->stage=3;
        else state->stage=(c==')') ? 1 : 0;
        break;
      default:
        if ((c==' ') || (c=='\t') || (c=='\r') || (c=='\n')) break;
        if (c=='{') return TRUE;
        state->stage=(c==')') ? 1 : 0;
        break;
    }
  }
  return FALSE;
}

static BOOLEAN yyLambdaRememberArrowState(const BOOLEAN may_have_arrow)
{
  currentVoice->lambda_arrow_state=
    may_have_arrow ? yyLambdaArrowMaybe : yyLambdaArrowNo;
  return may_have_arrow;
}

/* A source with no logical arrow cannot contain either lambda spelling.
   Remember that fact for regular files and for loop bodies, which Singular
   rewinds and parses many times.  One-shot procedure buffers keep the local
   checks so entering a procedure does not allocate a cache record. */
static BOOLEAN yyLambdaCurrentSourceMayHaveArrowSlow() YY_LAMBDA_COLD;
static BOOLEAN yyLambdaCurrentSourceMayHaveArrowSlow()
{
  if (currentVoice->lambda_arrow_state!=yyLambdaArrowUnknown)
    return currentVoice->lambda_arrow_state==yyLambdaArrowMaybe;

  yyLambdaArrowScanState scan={0};
  if (currentVoice->sw==BI_buffer)
  {
    if (currentVoice->buffer==NULL) return TRUE;
    const unsigned char *bytes=
      (const unsigned char *)currentVoice->buffer;
    size_t count=0;
    while ((count<yyLambdaBufferArrowScanLimit)&&(bytes[count]!='\0'))
      count++;
    if (yyLambdaArrowScanBytes(&scan,bytes,count))
      return yyLambdaRememberArrowState(TRUE);
    return yyLambdaRememberArrowState(
      (count==yyLambdaBufferArrowScanLimit));
  }

  if ((currentVoice->sw!=BI_file)||(currentVoice->files==NULL)||
      (currentVoice->ftellptr<=0))
    return TRUE;

  FILE *file=currentVoice->files;
  const int saved_errno=errno;
  const long saved_position=ftell(file);
  if (feof(file)||ferror(file)||(saved_position<0))
  {
    errno=saved_errno;
    return TRUE;
  }

  /* Checking the size avoids reading a large file merely to optimize it. */
  if (fseek(file,0,SEEK_END)!=0)
  {
    fseek(file,saved_position,SEEK_SET);
    errno=saved_errno;
    return TRUE;
  }
  const long file_size=ftell(file);
  if ((file_size<0)||(file_size>yyLambdaFileArrowScanLimit) ||
      (fseek(file,0,SEEK_SET)!=0))
  {
    const BOOLEAN restored=(fseek(file,saved_position,SEEK_SET)==0);
    errno=saved_errno;
    return restored ? yyLambdaRememberArrowState(TRUE) : TRUE;
  }

  BOOLEAN may_have_arrow=FALSE;
  unsigned char bytes[8192];
  size_t count;
  while ((!may_have_arrow)&&((count=fread(bytes,1,sizeof(bytes),file))>0))
    may_have_arrow=yyLambdaArrowScanBytes(&scan,bytes,count);
  const BOOLEAN scan_ok=!ferror(file);
  const BOOLEAN restored=(fseek(file,saved_position,SEEK_SET)==0);
  errno=saved_errno;
  if ((!scan_ok)||(!restored)) return TRUE;
  return yyLambdaRememberArrowState(may_have_arrow);
}

static const char *yyLambdaVoiceSuffix() YY_LAMBDA_OUTLINE;
static const char *yyLambdaVoiceSuffix()
{
  if ((currentVoice==NULL)||(currentVoice->buffer==NULL)||
      (currentVoice->fptr<0))
    return NULL;
  if ((currentVoice->fptr>0)&&
      (currentVoice->buffer[currentVoice->fptr-1]=='\0'))
    return NULL;
  return currentVoice->buffer+currentVoice->fptr;
}

static BOOLEAN yyLambdaAsciiWhitespace(const int c)
{
  return (c==' ') || (c=='\t') || (c=='\r') || (c=='\n');
}

static const char *yyLambdaFindByte(const char *pos, const char *end,
                                    const char needle)
{
  while ((pos<end)&&(*pos!=needle)) pos++;
  return (pos<end) ? pos : NULL;
}

enum yyLambdaSuffixResult
{
  yyLambdaSuffixIncomplete,
  yyLambdaSuffixMatch,
  yyLambdaSuffixReject
};

struct yyLambdaSuffixReader
{
  const char *flex_pos;
  const char *flex_end;
  const char *voice_pos;
  size_t remaining;
};

static int yyLambdaSuffixNext(yyLambdaSuffixReader *reader) YY_LAMBDA_OUTLINE;
static int yyLambdaSuffixNext(yyLambdaSuffixReader *reader)
{
  if (reader->remaining==0) return EOF;
  if (reader->flex_pos<reader->flex_end)
  {
    reader->remaining--;
    return (unsigned char)*reader->flex_pos++;
  }
  if ((reader->voice_pos!=NULL)&&(*reader->voice_pos!='\0'))
  {
    reader->remaining--;
    return (unsigned char)*reader->voice_pos++;
  }
  return EOF;
}

/* Classify only a bounded suffix.  An incomplete result always falls back to
   the side-effect-free full probe, including when an arrow crosses a buffer
   boundary or follows unusually long whitespace. */
static yyLambdaSuffixResult
yyLambdaClassifySuffix(const char *flex_pos, const char *flex_end,
                       const char *voice_pos)
{
  yyLambdaSuffixReader reader;
  reader.flex_pos=flex_pos;
  reader.flex_end=flex_end;
  reader.voice_pos=voice_pos;
  reader.remaining=64;

  int c;
  do { c=yyLambdaSuffixNext(&reader); }
  while ((c!=EOF)&&yyLambdaAsciiWhitespace(c));
  if (c==EOF) return yyLambdaSuffixIncomplete;
  if (c!='-') return yyLambdaSuffixReject;

  c=yyLambdaSuffixNext(&reader);
  if (c==EOF) return yyLambdaSuffixIncomplete;
  if (c!='>') return yyLambdaSuffixReject;

  do { c=yyLambdaSuffixNext(&reader); }
  while ((c!=EOF)&&yyLambdaAsciiWhitespace(c));
  if (c==EOF) return yyLambdaSuffixIncomplete;
  return (c=='{') ? yyLambdaSuffixMatch : yyLambdaSuffixReject;
}

enum yyLambdaFormalResult
{
  yyLambdaFormalContinue,
  yyLambdaFormalClose,
  yyLambdaFormalReject
};

enum yyLambdaFormalContext
{
  yyLambdaFormalTop,
  yyLambdaFormalDimension,
  yyLambdaFormalBacktick
};

enum yyLambdaFormalLexState
{
  yyLambdaFormalNormal,
  yyLambdaFormalWord,
  yyLambdaFormalString,
  yyLambdaFormalLineComment,
  yyLambdaFormalBlockComment,
  yyLambdaFormalSlash,
  yyLambdaFormalHash,
  yyLambdaFormalColon
};

enum yyLambdaFormalPhase
{
  yyLambdaFormalExpectFragment,
  yyLambdaFormalAfterFirstAtom,
  yyLambdaFormalAfterAliasType,
  yyLambdaFormalAfterName
};

struct yyLambdaFormalState
{
  int context;
  int return_context;
  int lex_state;
  int phase;
  int round_depth;
  int square_depth;
  size_t word_length;
  int block_previous;
  BOOLEAN first_is_alias;
  BOOLEAN last_atom_is_qualified;
  BOOLEAN dimension_seen;
  BOOLEAN qualifier_rhs;
  BOOLEAN word_is_alias;
  BOOLEAN escaped;
  BOOLEAN backtick_is_atom;
};

static BOOLEAN yyLambdaIdentifierStart(const int c)
{
  return ((c>='a')&&(c<='z')) || ((c>='A')&&(c<='Z')) ||
         (c=='@') || (c=='\'') || (c=='_');
}

static BOOLEAN yyLambdaIdentifierChar(const int c)
{
  return yyLambdaIdentifierStart(c) || ((c>='0')&&(c<='9'));
}

/* Keep the same conservative downstream identifier limit on every platform.
   OM_MAX_BLOCK_SIZE depends on the system page size, while the accepted
   language and its regression tests must not. */
static const size_t yyLambdaMaxFormalWord=1007;

static BOOLEAN yyLambdaFormalWordByte(yyLambdaFormalState *state, const int c)
{
  static const char alias_word[]="alias";
  if ((state->word_length>=5) ||
      (c!=alias_word[state->word_length]))
    state->word_is_alias=FALSE;
  /* Saturate one byte beyond the accepted limit.  Downstream identifiers are
     NUL-terminated and some cleanup paths assume a small allocation. */
  if (state->word_length<=yyLambdaMaxFormalWord) state->word_length++;
  return state->word_length<=yyLambdaMaxFormalWord;
}

static void yyLambdaFormalInit(yyLambdaFormalState *state)
{
  memset(state,0,sizeof(*state));
  state->context=yyLambdaFormalTop;
  state->return_context=yyLambdaFormalTop;
  state->lex_state=yyLambdaFormalNormal;
  state->phase=yyLambdaFormalExpectFragment;
}

static BOOLEAN yyLambdaFormalAtom(yyLambdaFormalState *state,
                                  const BOOLEAN is_alias)
{
  if (state->qualifier_rhs)
  {
    if (((state->phase!=yyLambdaFormalAfterFirstAtom) ||
         state->first_is_alias) &&
        (state->phase!=yyLambdaFormalAfterName))
      return FALSE;
    state->qualifier_rhs=FALSE;
    state->last_atom_is_qualified=TRUE;
    return TRUE;
  }

  switch (state->phase)
  {
    case yyLambdaFormalExpectFragment:
      state->phase=yyLambdaFormalAfterFirstAtom;
      state->first_is_alias=is_alias;
      state->last_atom_is_qualified=FALSE;
      return TRUE;
    case yyLambdaFormalAfterFirstAtom:
      if (state->last_atom_is_qualified) return FALSE;
      state->phase=state->first_is_alias
                   ? yyLambdaFormalAfterAliasType
                   : yyLambdaFormalAfterName;
      state->last_atom_is_qualified=FALSE;
      return TRUE;
    case yyLambdaFormalAfterAliasType:
      state->phase=yyLambdaFormalAfterName;
      state->last_atom_is_qualified=FALSE;
      return TRUE;
    default:
      return FALSE;
  }
}

static BOOLEAN yyLambdaFormalFragmentComplete(const yyLambdaFormalState *state)
{
  return !state->qualifier_rhs &&
         ((state->phase==yyLambdaFormalExpectFragment) ||
         ((state->phase==yyLambdaFormalAfterFirstAtom) &&
          !state->first_is_alias) ||
         (state->phase==yyLambdaFormalAfterName));
}

static yyLambdaFormalResult YY_LAMBDA_COLD
yyLambdaFormalFeed(yyLambdaFormalState *state, int c)
{
  for (;;)
  {
    if (state->lex_state==yyLambdaFormalWord)
    {
      if (yyLambdaIdentifierChar(c))
      {
        if (!yyLambdaFormalWordByte(state,c))
          return yyLambdaFormalReject;
        return yyLambdaFormalContinue;
      }
      const BOOLEAN is_alias=state->word_is_alias &&
                             (state->word_length==5);
      state->lex_state=yyLambdaFormalNormal;
      if (!yyLambdaFormalAtom(state,is_alias))
        return yyLambdaFormalReject;
      continue;
    }

    if (state->lex_state==yyLambdaFormalString)
    {
      if (state->escaped) state->escaped=FALSE;
      else if (c=='\\') state->escaped=TRUE;
      else if (c=='"') state->lex_state=yyLambdaFormalNormal;
      return yyLambdaFormalContinue;
    }

    if (state->lex_state==yyLambdaFormalLineComment)
    {
      if (c=='\n') state->lex_state=yyLambdaFormalNormal;
      return yyLambdaFormalContinue;
    }

    if (state->lex_state==yyLambdaFormalBlockComment)
    {
      if ((state->block_previous=='*')&&(c=='/'))
      {
        state->lex_state=yyLambdaFormalNormal;
        state->block_previous=0;
      }
      else
        state->block_previous=c;
      return yyLambdaFormalContinue;
    }

    if (state->lex_state==yyLambdaFormalSlash)
    {
      state->lex_state=yyLambdaFormalNormal;
      if (c=='/')
      {
        state->lex_state=yyLambdaFormalLineComment;
        return yyLambdaFormalContinue;
      }
      if (c=='*')
      {
        state->lex_state=yyLambdaFormalBlockComment;
        state->block_previous=0;
        return yyLambdaFormalContinue;
      }
      if (state->context==yyLambdaFormalTop)
        return yyLambdaFormalReject;
      continue;
    }

    if (state->lex_state==yyLambdaFormalHash)
    {
      state->lex_state=yyLambdaFormalNormal;
      if (c=='!')
      {
        state->lex_state=yyLambdaFormalLineComment;
        return yyLambdaFormalContinue;
      }
      if ((state->context==yyLambdaFormalTop) &&
          !yyLambdaFormalAtom(state,FALSE))
        return yyLambdaFormalReject;
      continue;
    }

    if (state->lex_state==yyLambdaFormalColon)
    {
      state->lex_state=yyLambdaFormalNormal;
      if (c!=':') return yyLambdaFormalReject;
      state->qualifier_rhs=TRUE;
      return yyLambdaFormalContinue;
    }

    if (state->context==yyLambdaFormalBacktick)
    {
      if (c=='`')
      {
        state->context=state->return_context;
        if (state->backtick_is_atom &&
            !yyLambdaFormalAtom(state,FALSE))
          return yyLambdaFormalReject;
        return yyLambdaFormalContinue;
      }
      if (c=='"')
      {
        state->lex_state=yyLambdaFormalString;
        state->escaped=FALSE;
      }
      else if (c=='/') state->lex_state=yyLambdaFormalSlash;
      else if (c=='#') state->lex_state=yyLambdaFormalHash;
      else if (c==';') return yyLambdaFormalReject;
      return yyLambdaFormalContinue;
    }

    if (state->context==yyLambdaFormalDimension)
    {
      if (c=='"')
      {
        state->lex_state=yyLambdaFormalString;
        state->escaped=FALSE;
      }
      else if (c=='/') state->lex_state=yyLambdaFormalSlash;
      else if (c=='#') state->lex_state=yyLambdaFormalHash;
      else if (c=='`')
      {
        state->return_context=yyLambdaFormalDimension;
        state->context=yyLambdaFormalBacktick;
        state->backtick_is_atom=FALSE;
      }
      else if (c==';') return yyLambdaFormalReject;
      else if (c=='(') state->round_depth++;
      else if (c==')')
      {
        if (state->round_depth==0) return yyLambdaFormalReject;
        state->round_depth--;
      }
      else if (c=='[') state->square_depth++;
      else if (c==']')
      {
        if ((state->square_depth==0) ||
            ((state->square_depth==1)&&(state->round_depth!=0)))
          return yyLambdaFormalReject;
        state->square_depth--;
        if (state->square_depth==0)
          state->context=yyLambdaFormalTop;
      }
      return yyLambdaFormalContinue;
    }

    /* Top-level formal-list syntax. */
    if ((c==' ')||(c=='\t')||(c=='\r')||(c=='\n'))
      return yyLambdaFormalContinue;
    if (yyLambdaIdentifierStart(c))
    {
      state->lex_state=yyLambdaFormalWord;
      state->word_length=0;
      state->word_is_alias=TRUE;
      continue;
    }
    if (c=='`')
    {
      state->return_context=yyLambdaFormalTop;
      state->context=yyLambdaFormalBacktick;
      state->backtick_is_atom=TRUE;
      return yyLambdaFormalContinue;
    }
    if (c=='/')
    {
      state->lex_state=yyLambdaFormalSlash;
      return yyLambdaFormalContinue;
    }
    if (c=='#')
    {
      state->lex_state=yyLambdaFormalHash;
      return yyLambdaFormalContinue;
    }
    if (c==':')
    {
      const BOOLEAN untyped_name=
        (state->phase==yyLambdaFormalAfterFirstAtom) &&
        !state->first_is_alias;
      if ((!untyped_name && (state->phase!=yyLambdaFormalAfterName)) ||
          state->dimension_seen || state->qualifier_rhs)
        return yyLambdaFormalReject;
      state->lex_state=yyLambdaFormalColon;
      return yyLambdaFormalContinue;
    }
    if (c==',')
    {
      if (!yyLambdaFormalFragmentComplete(state))
        return yyLambdaFormalReject;
      state->phase=yyLambdaFormalExpectFragment;
      state->first_is_alias=FALSE;
      state->last_atom_is_qualified=FALSE;
      state->dimension_seen=FALSE;
      return yyLambdaFormalContinue;
    }
    if (c=='[')
    {
      if (state->phase!=yyLambdaFormalAfterName)
        return yyLambdaFormalReject;
      state->context=yyLambdaFormalDimension;
      state->round_depth=0;
      state->square_depth=1;
      state->dimension_seen=TRUE;
      return yyLambdaFormalContinue;
    }
    if (c==')')
      return yyLambdaFormalFragmentComplete(state)
             ? yyLambdaFormalClose : yyLambdaFormalReject;
    return yyLambdaFormalReject;
  }
}

#ifdef HAVE_OMALLOC
#undef malloc
#define malloc omAlloc
#undef realloc
#define realloc omRealloc
#undef free
#define free omFree
#endif

static char * dupyytext()
{
  char* s;
  if (yyleng>0) yytext[yyleng-1] = '\0';
  s = omStrDup((char *)yytext);
  omMarkAsStaticAddr(s);
  return s;
}

static char * dupyytextNL()
{
  int i = yyleng;//strlen((char *)yytext);
  char * rc = (char*)omAlloc( 3 + i );
  omMarkAsStaticAddr(rc);
  if (i>0)
  {
    strncpy( rc, (char *)yytext, i-1 );
  }
  else
  {
    i++;
  }
  rc[i-1] = '\n';
  rc[i] = '\n';
  rc[i+1] = '\0';
  return rc;
}

  #undef YY_DECL
  #define YY_DECL int yylex(YYSTYPE* lvalp)

  #undef yywrap
  extern "C" {
  int yywrap() { return exitVoice(); }
  }

  #undef YY_INPUT
  #define YY_INPUT(buf,result,max_size) \
          result=feReadLine((char *)(buf),(max_size))

  #undef YY_USER_ACTION
  #define YY_USER_ACTION \
          if ((inerror==1)&&(*yytext>=' '))\
          { Print("   skipping text from `%s`",yytext);inerror=2; }

%}

digit          [0-9]
letter         [@a-zA-Z\']
integer        {digit}+
monom          {letter}+{digit}*
rgvars         ({digit}+[/])*{digit}+{monom}+
realnum        {digit}*"."{digit}+("e"[+-]{digit}+)?
name           ({letter}({letter}*{digit}*_*)*|_)
parname        #

/* %start START */
%option always-interactive

%x string
%x block
%x blockstr
%x brace
%x bracestr
%x bracket
%x asstring

%%
\/\/[^\n]*
^[ \r\t\n]*#![^\n]*
"/*"                     {
                           yy_noeof=noeof_comment;
                           yyLambdaSkipBlockComment();
                           yy_noeof=0;
                         }
while                    { prompt_char='.';
                           blocknest = 0; yy_noeof = noeof_brace; BEGIN(brace);
                           return WHILE_CMD;}
for                      { prompt_char='.';
                           blocknest = 0; yy_noeof = noeof_brace; BEGIN(brace);
                           return FOR_CMD;}

("help"|"?")[ \t\n]*     { yy_noeof = noeof_asstring;
                           BEGIN(asstring);
                           return HELP_CMD;
                         }

example[ \t\n]*          { yy_noeof = noeof_asstring;
                           BEGIN(asstring);
                           return EXAMPLE_CMD;
                         }

proc[ \t]+{name}[ \t]*\( {
                           char c; char *cp;
                           lvalp->name = omStrDup(iiProcName((char *)yytext,c,cp));
                           yy_noeof = noeof_procname;
                           blocknest = 1;
                           BEGIN(brace);
                           return PROC_DEF;
                         }
<asstring>[^;\n]+        {
                           lvalp->name = omStrDup((char *)yytext);
                           yy_noeof = 0; BEGIN(INITIAL);
                           return STRINGTOK;
                         }
<asstring>;              {
                           yy_noeof = 0; BEGIN(INITIAL);
                           return *yytext;
                         }

<brace>"\""              {
                           yy_noeof = noeof_string;
                           BEGIN(bracestr);
                           yymore();
                         }
<brace>"("               { if (blocknest++) yymore(); }
<brace>[^;\(\)]          { if (blocknest) yymore(); }
<brace>";"               {
                           if (blocknest)
                           {
                             lvalp->name = dupyytext();
                             return STRINGTOK;
                           }
                         }
<brace>")"               {
                           if (--blocknest <= 0)
                           {
                             yy_noeof = 0;
                             BEGIN(INITIAL);
                             lvalp->name = dupyytext();
                             return STRINGTOK;
                           }
                           yymore();
                         }
<bracestr>"\""           {
                           yy_noeof = noeof_brace;
                           BEGIN(brace);
                           yymore();
                         }
<bracestr>[^\"]          { yymore(); }
<bracket>"("             { return '('; }
<bracket>","             { return ','; }
<bracket>[ \t\n]*        { ; }
<bracket>[^\(\), \t\n]*  {
                           lvalp->name = omStrDup((char *)yytext);
                           return STRINGTOK;
                         }
<bracket>\"[^\"]*\"      {
                           lvalp->name = omStrDup((char *)yytext);
                           return STRINGTOK;
                         }
<bracket>")"             {
                           yy_noeof = 0; BEGIN(INITIAL);
                           return ')';
                         }

"{"                      {
                           yy_blocklineno = yylineno;
                           blocknest = 1;
                           yy_noeof = noeof_block;
                           BEGIN(block);
                         }
<block>"\""              {
                           yy_noeof = noeof_string;
                           BEGIN(blockstr);
                           yymore();
                         }
<blockstr>[^\"]          { yymore(); }
<blockstr>"\\\\"         { yymore(); }
<blockstr>"\\\""         { yymore(); }
<blockstr>"\""           {
                           yy_noeof = noeof_block;
                           BEGIN(block);
                           yymore();
                         }
<block>[^\{\}\"]*        { yymore(); }
<block>\/\/[^\n]*        { yymore(); }
<block>"{"               { blocknest++; yymore(); }
<block>"}"               {
                           if (--blocknest <= 0)
                           {
                             BEGIN(INITIAL);
                             yy_noeof = 0;
                             lvalp->name = dupyytextNL();
                             return BLOCKTOK;
                           }
                           yymore();
                         }
"\""                     { BEGIN(string); yy_noeof = noeof_string;}
~                        { return SYS_BREAK; }
<string>[^\"]            { yymore(); }
<string>"\\\\"           { yymore(); }
<string>"\\\""           { yymore(); }
<string>"\""             {
                           char * s;
                           yy_noeof = 0;
                           BEGIN(INITIAL);
                           s = lvalp->name = dupyytext();
                           while (*yytext)
                           {
                             if (*yytext == '\\') yytext++;
                             *s++ = *yytext++;
                           }
                           *s++ = *yytext++;
                           return STRINGTOK;
                         }

[ \t\r\n]                /* skip whitespace */
".."                     { return DOTDOT; }
"::"                     { return COLONCOLON; }
"--"                     { return MINUSMINUS; }
"++"                     { return PLUSPLUS  ; }
"=="                     { return EQUAL_EQUAL; }
"&&"                     { lvalp->i='&'; return LOGIC_OP; }
"||"                     { lvalp->i='|'; return LOGIC_OP; }
"<="                     { lvalp->i=LE; return COMP_OP; }
">="                     { lvalp->i=GE; return COMP_OP; }
"!"                      { return NOT; }
"!="                     { return NOTEQUAL; }
"<>"                     { return NOTEQUAL; }
"**"                     { return '^'; }
"->"                     {
                           int c;
                           char buf[1024];
                           int n = 0;
                           do
                           {
                             c = yyLambdaTrackedInput();
                             if (c == EOF) break;
                             if (n < (int)sizeof(buf)) buf[n++] = (char)c;
                           }
                           while ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'));
                           while (n > 0) unput(buf[--n]);
                           if (c == '{') return ARROW;
                           return ':';
                         }
\\                       { return '\\'; }
newline                  {
                           lvalp->name = omStrDup("\n");
                           return STRINGTOK;
                         }
{integer}                {
                           lvalp->name = (char *)yytext;
                           return INT_CONST;
                         }
{integer}\/{integer}     {
                           lvalp->name = (char *)yytext;
                           return MONOM;
                         }
\$                       {
                           m2_end(1);
                         }
(quit|exit)[ \t\n]*;     {
                           #ifdef MM_STAT
                           mmStat(-500);
                           #endif
                           #ifdef OM_TRACK
			   #ifndef SING_NDEBUG
                             omPrintUsedTrackAddrs(stdout, 10);
                           #endif
                           #endif
                           m2_end(1);
                         }

{rgvars}|{realnum}       {
                           lvalp->name = (char *)yytext;
                           return MONOM;
                         }
[0-9]+\."e"[+-][0-9]+    {
                           lvalp->name = (char *)yytext;
                           return MONOM;
                         }
[0-9]+\./[^\.]           {
                           lvalp->name = (char *)yytext;
                           return MONOM;
                         }

({parname}|{name})       {
                           /* {name} */
                           int rc=0;
                           if (yytext[strlen((char *)yytext)-1] == '\n')
                           {
                             yytext[strlen((char *)yytext)-1] = '\0';
                           }
                           if (yyleng > 1)
                           {
                             rc = IsCmd((char *)yytext,lvalp->i);
                             if (rc) return rc;
                           }
                           if (UNLIKELY(yyleng>=OM_MAX_BLOCK_SIZE))
                             lvalp->name=iiDupIdentifier((char *)yytext);
                           else
                             lvalp->name=omStrDup((char *)yytext);
                           return UNKNOWN_IDENT;
                         }

.                       {
                           /*if (*yytext == '\n') REJECT;*/
                           REGISTER char ch= *yytext;
                           lvalp->i = ch;
                           switch(ch)
                           {
                             case '(':
                               YY_LAMBDA_FAST_ORDINARY_PAREN();
                               return yyLambdaOpeningParenToken(lvalp);
                             /* case '&': */
                             case '|':
                               return LOGIC_OP;
                             /* case '/': */
                             case '%':
                             case '*':
                               return MULDIV_OP;
                             /* case '<': */
                             case '>':
                               return COMP_OP;
                             default:
                               break;
                            }
                            return ch;
                         }
%%

static int
yyLambdaOpeningParenToken(YYSTYPE *lvalp)
{
  if (yytext_ptr>yy_current_buffer->yy_ch_buf)
  {
    const unsigned int previous=(unsigned char)yytext_ptr[-1];
    if (LIKELY((previous==(unsigned int)'@') ||
               (previous==(unsigned int)'\'') ||
               (previous==(unsigned int)'_') ||
               (previous==(unsigned int)')') ||
               (previous==(unsigned int)']')))
      return '(';
  }

  /* Classify reusable sources before doing token-local lookahead.  This pays
     once for an arrow-free file or loop body and makes every later opening
     parenthesis take the one-check path in the scanner action. */
  if ((yy_current_buffer->yy_fill_buffer!=0) &&
      (currentVoice!=NULL) &&
      ((currentVoice->sw==BI_file) ||
       ((currentVoice->sw==BI_buffer) &&
        (currentVoice->typ==BT_break))) &&
      !yyLambdaCurrentSourceMayHaveArrowSlow())
    return '(';

  const int lambda_token=yyLambdaParamToken(lvalp);
  return (lambda_token!=0) ? lambda_token : '(';
}

static int
yyLambdaParamToken(YYSTYPE *lvalp)
{
  char *probe=yy_c_buf_p;
  char *probe_end=&yy_current_buffer->yy_ch_buf[yy_n_chars];
  int probe_char=EOF;

  if (probe<probe_end)
  {
    probe_char=(unsigned char)yy_hold_char;
    /* Most parenthesized expressions can be
       decided from their first byte.  Return before
       setting up any lookahead state for bytes that
       cannot begin a formal fragment. */
    if (!yyLambdaAsciiWhitespace(probe_char) &&
        !yyLambdaIdentifierStart(probe_char) &&
        (probe_char!=')') && (probe_char!=',') &&
        (probe_char!='`') && (probe_char!='#') &&
        (probe_char!='/'))
      return 0;
    if ((probe_char==')') &&
        (probe+1<probe_end) &&
        !yyLambdaAsciiWhitespace(
          (unsigned char)probe[1]) &&
        (probe[1]!='-'))
      return 0;
  }

  char *lambda_close=NULL;

  if (probe<probe_end)
  {
    while ((probe_char==' ')||(probe_char=='\t')||
           (probe_char=='\r')||(probe_char=='\n'))
    {
      probe++;
      if (probe>=probe_end) break;
      probe_char=(unsigned char)*probe;
    }
  }

  /* A single identifier in parentheses is a very
     common hot path (including generated for-loop
     conditions and bodies).  If the already
     buffered suffix cannot start "-> {", avoid
     speculative input altogether. */
  const BOOLEAN identifier_prefix=
    (probe<probe_end) &&
    yyLambdaIdentifierStart(probe_char);
  if (identifier_prefix && (probe+1<probe_end))
  {
    const int second=(unsigned char)probe[1];
    if (second==')')
      lambda_close=probe+1;
    else if (!yyLambdaIdentifierChar(second) &&
             !yyLambdaAsciiWhitespace(second) &&
             (second!=',') && (second!=':') &&
             (second!='[') &&
             (second!='`') && (second!='#') &&
             (second!='/'))
      return 0;
  }
  if (identifier_prefix && (lambda_close==NULL))
  {
    char *identifier=probe+1;
    while ((identifier<probe_end) &&
           yyLambdaIdentifierChar(
             (unsigned char)*identifier))
      identifier++;
    while ((identifier<probe_end) &&
           yyLambdaAsciiWhitespace(
             (unsigned char)*identifier))
      identifier++;
    if ((identifier<probe_end) &&
        (*identifier==')'))
      lambda_close=identifier;
    else if (identifier<probe_end)
    {
      const char next=*identifier;
      if (next=='/')
      {
        if ((identifier+1<probe_end) &&
            (identifier[1]!='/') &&
            (identifier[1]!='*'))
          return 0;
      }
      else if (next==':')
      {
        if ((identifier+1<probe_end) &&
            (identifier[1]!=':'))
          return 0;
      }
      else if ((next!=',') && (next!='`') &&
               (next!='#') && (next!='[') &&
               !yyLambdaIdentifierStart(next))
        return 0;
    }
  }

  /* Once obvious identifier expressions have been
     rejected, locate a close in ordinary lists with
     a minimal scan.  Parentheses, dimensions,
     comments, strings, and computed names fall back
     to the complete formal-state machine. */
  if ((lambda_close==NULL) &&
      (probe<probe_end))
  {
    char *simple=probe;
    int simple_char=probe_char;
    while (simple<probe_end)
    {
      if (simple_char==')')
      {
        lambda_close=simple;
        break;
      }
      if ((simple_char=='(') || (simple_char=='[') ||
          (simple_char=='/') || (simple_char=='"') ||
          (simple_char=='`') || (simple_char=='#'))
        break;
      simple++;
      if (simple<probe_end)
        simple_char=(unsigned char)*simple;
    }
  }

  if ((!identifier_prefix) &&
      (lambda_close==NULL) &&
           (probe<probe_end))
  {
    /* Reject ordinary parenthesized expressions
       from the first buffered byte whenever it
       cannot begin a formal parameter.  A slash is
       only ambiguous at the buffer boundary or
       when it starts a comment. */
    if (probe_char==')')
      lambda_close=probe;
    else if (probe_char=='/')
    {
      if ((probe+1<probe_end) &&
          (probe[1]!='/') && (probe[1]!='*'))
        return 0;
    }
    else if ((probe_char!=')') &&
             (probe_char!=',') &&
             (probe_char!='`') &&
             (probe_char!='#'))
      return 0;
  }

  /* A complete one-line header already present in flex's buffer can be
     validated and consumed in one pass.  The general probe/consume path
     below is still required for buffer boundaries, multiline headers and
     sources whose input state has to be captured for replay. */
  if (lambda_close!=NULL)
  {
    const char *opening_brace=lambda_close+1;
    BOOLEAN one_line=TRUE;
    while ((opening_brace<probe_end) &&
           yyLambdaAsciiWhitespace((unsigned char)*opening_brace))
    {
      if ((*opening_brace=='\n') || (*opening_brace=='\r'))
        one_line=FALSE;
      opening_brace++;
    }
    if (one_line && (opening_brace+1<probe_end) &&
        (opening_brace[0]=='-') && (opening_brace[1]=='>'))
    {
      opening_brace+=2;
      while ((opening_brace<probe_end) &&
             yyLambdaAsciiWhitespace((unsigned char)*opening_brace))
      {
        if ((*opening_brace=='\n') || (*opening_brace=='\r'))
          one_line=FALSE;
        opening_brace++;
      }
      if (one_line && (opening_brace<probe_end) &&
          (*opening_brace=='{'))
      {
        yyLambdaFormalState formal;
        yyLambdaFormalResult formal_result=yyLambdaFormalContinue;
        const char *formal_pos=probe;
        yyLambdaFormalInit(&formal);
        while ((formal_pos<=lambda_close) &&
               (formal_result==yyLambdaFormalContinue))
        {
          const int formal_char=(formal_pos==probe)
                                ? probe_char
                                : (unsigned char)*formal_pos;
          if ((formal_char=='\n') || (formal_char=='\r'))
            one_line=FALSE;
          formal_result=yyLambdaFormalFeed(&formal,formal_char);
          formal_pos++;
        }
        if (one_line && (formal_result==yyLambdaFormalClose) &&
            (formal_pos==lambda_close+1))
        {
          const size_t param_len=
            (size_t)(lambda_close-(yytext_ptr+1));
          *yy_c_buf_p=yy_hold_char;
          if (yyLambdaPendingParams!=NULL)
            omFree((ADDRESS)yyLambdaPendingParams);
          lvalp->name=(char *)omAlloc(param_len+1);
          if (param_len>0)
            memcpy(lvalp->name,yytext_ptr+1,param_len);
          lvalp->name[param_len]='\0';
          yyLambdaPendingParams=lvalp->name;

          /* Match the state produced by reading through the opening brace,
             then expose a synthetic arrow to the established production. */
          yy_c_buf_p=(char *)opening_brace+1;
          yy_hold_char=*yy_c_buf_p;
          *yy_c_buf_p='\0';
          yyLambdaLeaveOpeningBrace();
          unput('>');
          unput('-');
          return UNKNOWN_IDENT;
        }
      }
    }
  }

  if (lambda_close!=NULL)
  {
    const char *suffix=lambda_close+1;
    /* A non-whitespace byte other than '-' cannot
       start an arrow.  Keep this overwhelmingly
       common proc-call/expression path inline. */
    if ((suffix<probe_end) &&
        !yyLambdaAsciiWhitespace(
          (unsigned char)*suffix) &&
        (*suffix!='-'))
      return 0;
    else
    {
      const char *voice_suffix=
        (yy_current_buffer->yy_fill_buffer!=0)
        ? yyLambdaVoiceSuffix() : NULL;
      if ((suffix==probe_end) &&
          (voice_suffix!=NULL))
      {
        const char *voice=voice_suffix;
        while ((*voice!='\0') &&
               yyLambdaAsciiWhitespace(
                 (unsigned char)*voice))
          voice++;
        if ((*voice!='\0') && (*voice!='-'))
          return 0;
      }
      if (yyLambdaClassifySuffix(suffix,probe_end,
                                 voice_suffix)==
          yyLambdaSuffixReject)
        return 0;
    }
  }

  const yyLambdaProbeResult lambda_probe=
    yyLambdaProbe();

  if ((lambda_probe==yyLambdaProbeMatch) ||
      (lambda_probe==yyLambdaProbeUnavailable))
  {
    int c=EOF;
    yyLambdaFormalState formal;
    yyLambdaFormalResult formal_result=
      yyLambdaFormalContinue;
    size_t param_len=0;
    const BOOLEAN capture=
      (lambda_probe==yyLambdaProbeUnavailable);
    yyLambdaFormalInit(&formal);
    if (capture) yyLambdaCaptureBegin();

  /* A seekable or in-memory source was already
     classified without side effects.  For a
     non-seekable source that crosses the current
     buffer, capture the line-state changes while
     flex preserves the speculative bytes. */
  while (formal_result==yyLambdaFormalContinue)
  {
    c=capture ? yyLambdaCaptureInput()
              : yyLambdaInput();
    if (c==EOF) break;
    formal_result=yyLambdaFormalFeed(&formal,c);
  }

  if ((c!=EOF)&&
      (formal_result==yyLambdaFormalClose))
  {
    param_len=(size_t)(yy_c_buf_p-yytext_ptr-2);
    do
    {
      c=capture ? yyLambdaCaptureInput()
                : yyLambdaInput();
    }
    while ((c==' ')||(c=='\t')||(c=='\r')||(c=='\n'));
    if (c=='-')
      c=capture ? yyLambdaCaptureInput()
                : yyLambdaInput();
    else c=EOF;
    if (c=='>')
      c=capture ? yyLambdaCaptureInput()
                : yyLambdaInput();
    else c=EOF;
    while ((c==' ')||(c=='\t')||(c=='\r')||(c=='\n'))
      c=capture ? yyLambdaCaptureInput()
                : yyLambdaInput();
    if (c=='{')
    {
      if (yyLambdaPendingParams!=NULL)
        omFree((ADDRESS)yyLambdaPendingParams);
      lvalp->name=(char *)omAlloc(param_len+1);
      if (param_len>0)
        memcpy(lvalp->name,yytext_ptr+1,param_len);
      lvalp->name[param_len]='\0';
      yyLambdaPendingParams=lvalp->name;
      if (capture) yyLambdaCaptureDiscard();
      yyLambdaLeaveOpeningBrace();
      /* Feed the existing extendedid ARROW BLOCKTOK production.  The input
         arrow has already been consumed, so put back a synthetic one before
         the opening brace left by yyLambdaLeaveOpeningBrace(). */
      unput('>');
      unput('-');
      return UNKNOWN_IDENT;
    }
  }
    if (capture)
      yyLambdaCaptureReplay();
    else
      yyLambdaRewind();
  }
  return 0;
}

struct yyLambdaLineEvent
{
  size_t offset;
  int lineno;
  char linebuf[80];
};

struct yyLambdaCaptureState
{
  yyLambdaLineEvent *events;
  size_t event_count;
  size_t event_capacity;
  size_t offset;
  int start_lineno;
  int last_lineno;
  char start_linebuf[80];
  char last_linebuf[80];
  BOOLEAN active;
};

struct yyLambdaReplayState
{
  Voice *voice;
  char *replay_buffer;
  char *saved_buffer;
  long saved_fptr;
  int saved_buff_size;
  feBufferInputs saved_sw;
  yyLambdaLineEvent *events;
  size_t event_count;
  size_t next_event;
  yyLambdaReplayState *next;
};

static yyLambdaCaptureState yyLambdaCapture={0};

static void yyLambdaSetLineState(const int line, const char *linebuf)
{
  yylineno=line;
  memcpy(my_yylinebuf,linebuf,sizeof(my_yylinebuf));
}

static void yyLambdaCaptureAddEvent(const size_t offset)
{
  if (yyLambdaCapture.event_count==yyLambdaCapture.event_capacity)
  {
    const size_t old_capacity=yyLambdaCapture.event_capacity;
    const size_t new_capacity=(old_capacity==0) ? 8 : old_capacity*2;
    const size_t old_size=old_capacity*sizeof(yyLambdaLineEvent);
    const size_t new_size=new_capacity*sizeof(yyLambdaLineEvent);
    if (yyLambdaCapture.events==NULL)
      yyLambdaCapture.events=(yyLambdaLineEvent *)omAlloc(new_size);
    else
      yyLambdaCapture.events=(yyLambdaLineEvent *)
        omReallocSize(yyLambdaCapture.events,old_size,new_size);
    yyLambdaCapture.event_capacity=new_capacity;
  }
  yyLambdaLineEvent *event=
    &yyLambdaCapture.events[yyLambdaCapture.event_count++];
  event->offset=offset;
  event->lineno=yylineno;
  memcpy(event->linebuf,my_yylinebuf,sizeof(event->linebuf));
}

static void yyLambdaCaptureBegin()
{
  if (yyLambdaCapture.events!=NULL)
    omFree((ADDRESS)yyLambdaCapture.events);
  memset(&yyLambdaCapture,0,sizeof(yyLambdaCapture));
  yyLambdaCapture.start_lineno=yylineno;
  yyLambdaCapture.last_lineno=yylineno;
  memcpy(yyLambdaCapture.start_linebuf,my_yylinebuf,
         sizeof(yyLambdaCapture.start_linebuf));
  memcpy(yyLambdaCapture.last_linebuf,my_yylinebuf,
         sizeof(yyLambdaCapture.last_linebuf));
  yyLambdaCapture.active=TRUE;
}

static int yyLambdaCaptureInput()
{
  const size_t offset=yyLambdaCapture.offset;
  const int c=yyLambdaInput();

  /* Input-boundary replay applies an older replay's line event before
     returning the corresponding bytes, so the state observed here is final. */
  if ((yylineno!=yyLambdaCapture.last_lineno) ||
      (memcmp(my_yylinebuf,yyLambdaCapture.last_linebuf,
              sizeof(yyLambdaCapture.last_linebuf))!=0))
  {
    yyLambdaCaptureAddEvent(offset);
    yyLambdaCapture.last_lineno=yylineno;
    memcpy(yyLambdaCapture.last_linebuf,my_yylinebuf,
           sizeof(yyLambdaCapture.last_linebuf));
  }
  if (c!=EOF)
    yyLambdaCapture.offset++;
  return c;
}

static void yyLambdaCaptureDiscard()
{
  if (yyLambdaCapture.events!=NULL)
    omFree((ADDRESS)yyLambdaCapture.events);
  memset(&yyLambdaCapture,0,sizeof(yyLambdaCapture));
}

static void yyLambdaCaptureReplay()
{
  if (!yyLambdaCapture.active)
  {
    yyLambdaRewind();
    return;
  }

  *yy_c_buf_p=yy_hold_char;
  YY_BUFFER_STATE old_buffer=YY_CURRENT_BUFFER;
  char *first=yytext_ptr+1;
  char *last=old_buffer->yy_ch_buf+yy_n_chars;
  const size_t replay_length=(last>=first) ? (size_t)(last-first) : 0;
  if (replay_length>(size_t)INT_MAX)
    YY_FATAL_ERROR("lambda replay buffer exceeds flex's integer limit");

  if (replay_length==0)
  {
    yyLambdaCaptureDiscard();
    yyLambdaRewind();
    return;
  }

  char *replay_text=(char *)omAlloc(replay_length+1);
  memcpy(replay_text,first,replay_length);
  replay_text[replay_length]='\0';

  yyLambdaReplayState *replay=(yyLambdaReplayState *)
    omAlloc0(sizeof(yyLambdaReplayState));
  replay->voice=currentVoice;
  replay->replay_buffer=replay_text;
  replay->saved_buffer=currentVoice->buffer;
  replay->saved_fptr=currentVoice->fptr;
  replay->saved_buff_size=currentVoice->buff_size;
  replay->saved_sw=currentVoice->sw;
  replay->events=yyLambdaCapture.events;
  replay->event_count=yyLambdaCapture.event_count;
  replay->next=yyLambdaReplays;
  yyLambdaReplays=replay;

  currentVoice->buffer=replay_text;
  currentVoice->fptr=0;
  currentVoice->buff_size=(int)replay_length+1;
  currentVoice->sw=BI_replay;

  /* The underlying voice has advanced through the copied bytes.  Refill the
     existing flex buffer from the replay text, then restore that voice in
     feReadLine when the replay is exhausted.  Keeping one flex buffer makes
     manual yyinput() scans safe across this boundary. */
  yy_flush_buffer(old_buffer);
  yyLambdaSetLineState(yyLambdaCapture.start_lineno,
                       yyLambdaCapture.start_linebuf);
  yyLambdaCapture.events=NULL;
  memset(&yyLambdaCapture,0,sizeof(yyLambdaCapture));
}

static void yyLambdaErrorUserAction()
{
  /* Horizontal whitespace is grouped for throughput.  Preserve the old
     one-byte recovery diagnostic. */
  const BOOLEAN one_space=(yyleng>1)&&(*yytext==' ');
  const char saved=one_space ? yytext[1] : '\0';
  if (one_space) yytext[1]='\0';
  Print("   skipping text from `%s`",yytext);
  if (one_space) yytext[1]=saved;
  inerror=2;
}

void yyLambdaReplayLine(long offset)
{
  yyLambdaReplayState *replay=yyLambdaReplays;
  if ((replay==NULL)||(replay->voice!=currentVoice)) return;
  while ((replay->next_event<replay->event_count) &&
         (replay->events[replay->next_event].offset<=(size_t)offset))
  {
    const yyLambdaLineEvent *event=
      &replay->events[replay->next_event++];
    yyLambdaSetLineState(event->lineno,event->linebuf);
  }
  if ((replay->events!=NULL)&&
      (replay->next_event==replay->event_count))
  {
    omFree((ADDRESS)replay->events);
    replay->events=NULL;
  }
}

void yyLambdaReplayFinishInput()
{
  yyLambdaReplayState *replay=yyLambdaReplays;
  if ((replay==NULL)||(replay->voice!=currentVoice)) return;
  yyLambdaReplays=replay->next;
  currentVoice->buffer=replay->saved_buffer;
  currentVoice->fptr=replay->saved_fptr;
  currentVoice->buff_size=replay->saved_buff_size;
  currentVoice->sw=replay->saved_sw;
  if (replay->replay_buffer!=NULL)
    omFree((ADDRESS)replay->replay_buffer);
  if (replay->events!=NULL) omFree((ADDRESS)replay->events);
  omFree((ADDRESS)replay);
}

static void yyLambdaReplayDiscardAll()
{
  while (yyLambdaReplays!=NULL)
  {
    yyLambdaReplayState *replay=yyLambdaReplays;
    yyLambdaReplays=replay->next;
    if ((replay->voice==currentVoice) &&
        (currentVoice->buffer==replay->replay_buffer))
    {
      currentVoice->buffer=replay->saved_buffer;
      currentVoice->fptr=replay->saved_fptr;
      currentVoice->buff_size=replay->saved_buff_size;
      currentVoice->sw=replay->saved_sw;
    }
    if (replay->replay_buffer!=NULL)
      omFree((ADDRESS)replay->replay_buffer);
    if (replay->events!=NULL) omFree((ADDRESS)replay->events);
    omFree((ADDRESS)replay);
  }
}

static int yyLambdaTrackedInput()
{
  return yyinput();
}

static void yyLambdaSkipBlockComment()
{
  BOOLEAN previous_star=FALSE;
  for (;;)
  {
    *yy_c_buf_p=yy_hold_char;
    char *pos=yy_c_buf_p;
    char *end=&yy_current_buffer->yy_ch_buf[yy_n_chars];

    if (previous_star && (pos<end))
    {
      if (*pos=='/')
      {
        pos++;
        yy_c_buf_p=pos;
        yy_hold_char=*pos;
        *pos='\0';
        return;
      }
      previous_star=FALSE;
    }

    while (pos<end)
    {
      const char *found=yyLambdaFindByte(pos,end,'*');
      if (found==NULL)
      {
        pos=end;
        break;
      }
      char *star=(char *)found;
      if (star+1<end)
      {
        if (star[1]=='/')
        {
          pos=star+2;
          yy_c_buf_p=pos;
          yy_hold_char=*pos;
          *pos='\0';
          return;
        }
        pos=star+1;
      }
      else
      {
        pos=end;
        previous_star=TRUE;
      }
    }

    yy_c_buf_p=pos;
    yy_hold_char=*pos;
    *pos='\0';
    const int c=yyLambdaTrackedInput();
    if (c==EOF) return;
    if (previous_star && (c=='/')) return;
    previous_star=(c=='*');
  }
}

struct yyLambdaProbeReader
{
  const char *flex_first;
  const char *flex_pos;
  const char *flex_end;
  const char *voice_pos;
  int input_kind;
  FILE *file;
  long file_offset;
  long saved_file_pos;
  int saved_errno;
  int file_pending;
  BOOLEAN has_file_pending;
  BOOLEAN file_probe_started;
  BOOLEAN restore_file_pos;
  BOOLEAN restore_errno;
  yyLambdaProbeResult exhaustion;
};

/* Advance over state-preserving runs in the current contiguous source span.
   The first flex byte is special because flex stores it in yy_hold_char; it is
   deliberately left to yyLambdaProbeNext.  Terminators that change formal
   state are likewise left for yyLambdaFormalFeed. */
static yyLambdaFormalResult
yyLambdaProbeSkipFormal(yyLambdaProbeReader *reader,
                        yyLambdaFormalState *state)
{
  const char *pos;
  const char *end=NULL;
  BOOLEAN in_voice=FALSE;

  if (reader->flex_pos<reader->flex_end)
  {
    if (reader->flex_pos==reader->flex_first)
      return yyLambdaFormalContinue;
    pos=reader->flex_pos;
    end=reader->flex_end;
  }
  else
  {
    if ((yy_current_buffer->yy_fill_buffer==0) ||
        (reader->voice_pos==NULL) || (*reader->voice_pos=='\0'))
      return yyLambdaFormalContinue;
    pos=reader->voice_pos;
    in_voice=TRUE;
  }

  const char *start=pos;
  if (state->lex_state==yyLambdaFormalWord)
  {
    while ((in_voice ? (*pos!='\0') : (pos<end)) &&
           yyLambdaIdentifierChar((unsigned char)*pos))
    {
      if (!yyLambdaFormalWordByte(state,(unsigned char)*pos))
        return yyLambdaFormalReject;
      pos++;
    }
  }
  else if (state->lex_state==yyLambdaFormalNormal)
  {
    while ((in_voice ? (*pos!='\0') : (pos<end)) &&
           yyLambdaAsciiWhitespace((unsigned char)*pos))
      pos++;
  }
  else if (state->lex_state==yyLambdaFormalLineComment)
  {
    /* Leave the newline so FormalFeed can return to normal state. */
    if (in_voice)
    {
      const char *newline=strchr(pos,'\n');
      pos=(newline==NULL) ? pos+strlen(pos) : newline;
    }
    else
    {
      const char *newline=yyLambdaFindByte(pos,end,'\n');
      pos=(newline==NULL) ? end : (const char *)newline;
    }
  }
  else if ((state->lex_state==yyLambdaFormalBlockComment) &&
           (state->block_previous!='*'))
  {
    /* Leave every star so FormalFeed can recognize the closing star-slash. */
    if (in_voice)
    {
      const char *star=strchr(pos,'*');
      pos=(star==NULL) ? pos+strlen(pos) : star;
    }
    else
    {
      const char *star=yyLambdaFindByte(pos,end,'*');
      pos=(star==NULL) ? end : (const char *)star;
    }
    if (pos>start) state->block_previous=(unsigned char)pos[-1];
  }

  if (pos==start) return yyLambdaFormalContinue;
  if (in_voice) reader->voice_pos=pos;
  else reader->flex_pos=pos;
  return yyLambdaFormalContinue;
}

static int yyLambdaProbeNext(yyLambdaProbeReader *reader) YY_LAMBDA_OUTLINE;
static int yyLambdaProbeNext(yyLambdaProbeReader *reader)
{
  if (reader->flex_pos<reader->flex_end)
  {
    const int c=(reader->flex_pos==reader->flex_first)
                ? (unsigned char)yy_hold_char
                : (unsigned char)*reader->flex_pos;
    reader->flex_pos++;
    return c;
  }

  /* A flex scan buffer is self-contained.  Falling through to the active
     voice here could splice an unrelated parent voice onto yy_scan_* input. */
  if (yy_current_buffer->yy_fill_buffer==0)
  {
    reader->exhaustion=yyLambdaProbeNoMatch;
    return EOF;
  }

  if ((reader->voice_pos!=NULL)&&(*reader->voice_pos!='\0'))
    return (unsigned char)*reader->voice_pos++;

  if (reader->input_kind==BI_file)
  {
    if (!reader->file_probe_started)
    {
      reader->file_probe_started=TRUE;
      reader->saved_errno=errno;
      reader->restore_errno=TRUE;
      /* ftellptr==0 is Singular's sentinel for a non-seekable BI_file
         (including redirected stdin after ftell fails). */
      if ((reader->file==NULL)||(reader->file_offset<=0) ||
          feof(reader->file)||ferror(reader->file) ||
          ((reader->saved_file_pos=ftell(reader->file))<0))
      {
        reader->exhaustion=yyLambdaProbeUnavailable;
        return EOF;
      }
      reader->restore_file_pos=TRUE;
      if (fseek(reader->file,reader->file_offset,SEEK_SET)!=0)
      {
        reader->exhaustion=yyLambdaProbeUnavailable;
        return EOF;
      }
    }

    for (;;)
    {
      int c;
      if (reader->has_file_pending)
      {
        c=reader->file_pending;
        reader->has_file_pending=FALSE;
      }
      else
        c=fgetc(reader->file);

      if (c==EOF)
      {
        reader->exhaustion=ferror(reader->file)
                           ? yyLambdaProbeUnavailable
                           : yyLambdaProbeNoMatch;
        return EOF;
      }

      /* feReadLine removes a backslash immediately followed by a newline
         before flex sees the next physical line.  Apply the same filter while
         peeking beyond its current in-memory line. */
      if (c=='\\')
      {
        const int next=fgetc(reader->file);
        if (next=='\n') continue;
        if (next!=EOF)
        {
          reader->file_pending=next;
          reader->has_file_pending=TRUE;
        }
      }
      return c;
    }
  }

  reader->exhaustion=(reader->input_kind==BI_buffer)
                     ? yyLambdaProbeNoMatch
                     : yyLambdaProbeUnavailable;
  return EOF;
}

static BOOLEAN yyLambdaProbeFinish(yyLambdaProbeReader *reader)
{
  BOOLEAN restored=TRUE;
  if (reader->restore_file_pos)
  {
    restored=(fseek(reader->file,reader->saved_file_pos,SEEK_SET)==0);
  }
  if (reader->restore_errno) errno=reader->saved_errno;
  return restored;
}

static yyLambdaProbeResult yyLambdaProbe()
{
  yyLambdaProbeReader reader;
  reader.flex_first=yy_c_buf_p;
  reader.flex_pos=yy_c_buf_p;
  reader.flex_end=&yy_current_buffer->yy_ch_buf[yy_n_chars];
  reader.voice_pos=NULL;
  reader.input_kind=0;
  reader.file=NULL;
  reader.file_offset=-1;
  reader.saved_errno=0;
  reader.file_pending=EOF;
  reader.has_file_pending=FALSE;
  reader.file_probe_started=FALSE;
  reader.restore_file_pos=FALSE;
  reader.restore_errno=FALSE;
  reader.exhaustion=yyLambdaProbeUnavailable;

  if (currentVoice!=NULL)
  {
    reader.voice_pos=yyLambdaVoiceSuffix();
    reader.input_kind=currentVoice->sw;
    reader.file=currentVoice->files;
    reader.file_offset=currentVoice->ftellptr;
  }

  yyLambdaProbeResult result=yyLambdaProbeNoMatch;
  yyLambdaFormalState formal;
  yyLambdaFormalResult formal_result=yyLambdaFormalContinue;
  int c;
  yyLambdaFormalInit(&formal);

  while (formal_result==yyLambdaFormalContinue)
  {
    formal_result=yyLambdaProbeSkipFormal(&reader,&formal);
    if (formal_result==yyLambdaFormalReject) break;
    c=yyLambdaProbeNext(&reader);
    if (c==EOF)
    {
      result=reader.exhaustion;
      goto probe_done;
    }
    formal_result=yyLambdaFormalFeed(&formal,c);
  }
  if (formal_result==yyLambdaFormalReject) goto probe_done;

  do
  {
    c=yyLambdaProbeNext(&reader);
    if (c==EOF)
    {
      result=reader.exhaustion;
      goto probe_done;
    }
  }
  while ((c==' ')||(c=='\t')||(c=='\r')||(c=='\n'));

  if (c!='-') goto probe_done;
  c=yyLambdaProbeNext(&reader);
  if (c==EOF)
  {
    result=reader.exhaustion;
    goto probe_done;
  }
  if (c!='>') goto probe_done;

  do
  {
    c=yyLambdaProbeNext(&reader);
    if (c==EOF)
    {
      result=reader.exhaustion;
      goto probe_done;
    }
  }
  while ((c==' ')||(c=='\t')||(c=='\r')||(c=='\n'));

  if (c=='{') result=yyLambdaProbeMatch;

probe_done:
  if (!yyLambdaProbeFinish(&reader))
    return yyLambdaProbeUnavailable;
  return result;
}

void * myynewbuffer()
{
  void * oldb = YY_CURRENT_BUFFER;
  yy_switch_to_buffer(yy_create_buffer(NULL, YY_BUF_SIZE));
  return oldb;
}

void myyoldbuffer(void * oldb)
{
  yy_delete_buffer(YY_CURRENT_BUFFER);
  yy_switch_to_buffer((YY_BUFFER_STATE)oldb);
  //yy_flush_buffer((YY_BUFFER_STATE)oldb);
}

void myychangebuffer()
{
  yy_flush_buffer((YY_BUFFER_STATE)YY_CURRENT_BUFFER);
  yy_delete_buffer(YY_CURRENT_BUFFER);
  yy_switch_to_buffer(yy_create_buffer(NULL, YY_BUF_SIZE));
}

void my_yy_flush()
{
  if (currentVoice!=NULL)
    currentVoice->lambda_arrow_state=yyLambdaArrowUnknown;
  yyLambdaReplayDiscardAll();
  YY_FLUSH_BUFFER;
  BEGIN(0);
}

static int yyLambdaInput()
{
  int c;
  *yy_c_buf_p=yy_hold_char;
  if (*yy_c_buf_p==YY_END_OF_BUFFER_CHAR)
  {
    if (yy_c_buf_p<&yy_current_buffer->yy_ch_buf[yy_n_chars])
      *yy_c_buf_p='\0';
    else
    {
      const int offset=(int)(yy_c_buf_p-yytext_ptr);
      ++yy_c_buf_p;
      const int action=yy_get_next_buffer();
      if (action!=EOB_ACT_CONTINUE_SCAN)
      {
        yy_c_buf_p=yytext_ptr+offset;
        yy_hold_char=*yy_c_buf_p;
        *yy_c_buf_p='\0';
        /* The speculative read must not pre-arm EOF.  In particular, an
           unfinished parenthesis inside execute() still has to return to its
           parent voice through the scanner's ordinary EOF path. */
        if ((yy_current_buffer->yy_fill_buffer!=0) &&
            (currentVoice!=NULL) && (currentVoice->prev!=NULL))
          yy_current_buffer->yy_buffer_status=YY_BUFFER_NORMAL;
        return EOF;
      }
      yy_c_buf_p=yytext_ptr+offset;
    }
  }
  c=*(unsigned char *)yy_c_buf_p;
  ++yy_c_buf_p;
  yy_hold_char=*yy_c_buf_p;
  *yy_c_buf_p='\0';
  return c;
}

static void yyLambdaRewind()
{
  *yy_c_buf_p=yy_hold_char;
  yy_c_buf_p=yytext_ptr+1;
  yy_hold_char=*yy_c_buf_p;
  *yy_c_buf_p='\0';
}

static void yyLambdaLeaveOpeningBrace()
{
  *yy_c_buf_p=yy_hold_char;
  --yy_c_buf_p;
  yy_hold_char=*yy_c_buf_p;
  *yy_c_buf_p='\0';
}
