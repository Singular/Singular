#ifndef SDDEBUG_H
#define SDDEBUG_H

#include <kernel/kutil.h>

/* file:        SDDebug.h
 * authors:     Grischa Studzinski & Benjamin Schnitzler
 * created:     ask git
 * last change: ask git
 *
 * DESCRIPTION:
 * This file contains methods for formatted (and indented)
 * debugging output. Flags can be set when using any function
 * with debug-output. It will then only output, if this flag
 * was set when invoking the freegbdvc command.
 *
 * REMARK:
 * We are trying to get rid of debug output in the code and
 * want to use gdb instead. We are currently working on a
 * method for gdb, which can break on a line by regular
 * expression, which will allow us to put debugging output
 * to the right parts of our code. However this is only working
 * with a 'recent' version of gdb.
 *
 * TODO: 
 * - Introduce a singular option for setting debogri whilst in
 *   debug mode.
 * - Create a function, which can turn on/off debug output for
 *   all debogris, local to the current function (works
 *   together with initdebogri?)
 */

/* currently used flags:
 * Attention:
 * The usage of DEBOGRI as flag-variable became obsolete.
 * ShiftDVec::deBoGri is the successor.
 *
 * 0    : RECOMMENDED for not-developers
 *        no debugging output; removes all in DEBOGRI defined
 *        debug functions from the code, which is recommended,
 *        if you do not want to debug the code.
 * 1    : entering of pairs - check degree violation
 * 2    : Which pairs are sorted out by... some criterion?
 * 4    : debugging of Gebauer-Moeller
 * 8    : debugging of the initialization of LObjects
 * 16   : debugging of lcm related things
 * 32   : debugging of missing pairs
 * 64   : debugging of chainCrit
 * 128  : Which pairs are sorted out by some criterion?
 *        - at the moment, this considers only GM3.
 * 256  : debugging divisiblility/ reduction related things
 * 512  : debugging Creation of DVecs
 * 1024 : Debugging memory corruption
 * 2048 : debugging redTail related things
 * 4096 : debugging the multiplication
 */

#ifdef KDEBUG
#define DEBOGRI 1
#endif

#if DEBOGRI > 0

//This has to be changed to make it work again
//void debugPrintDegViolation
//  (const char* where, poly p1, uint shift, poly p2, kStrategy);
#define debugPrintDegViolation(x1, x2, x3, x4, x5) ((void) 0)

namespace ShiftDVec
{
  namespace SD = ShiftDVec;

  class DeBoGri;

  void ExtensionTTest(sTObject* T, int tl);
  void DegreeTTest(sTObject* T, int tl);

  bool deBoGriPrint
    ( const char* description, 
      uint flag, bool assume = false, int indent = -1 );
  bool deBoGriPrint
    ( int number, const char* description, 
      uint flag, bool assume = false, int indent = -1 );
  bool deBoGriPrint
    ( void* address, const char* description, 
      uint flag, bool assume = false, int indent = -1 );
  bool deBoGriPrint
    ( const poly p, const char* description, 
      uint flag, bool assume = false, int indent = -1 );
  bool deBoGriPrint
    ( const poly p,
      const char* description,
      uint flag, const ring lmRing,
      const ring tlRing = NULL,
      bool assume = false, int indent = -1 );
  bool deBoGriPrint
    ( const TObject* P,
      const char* description, uint flag,
      const ring lmRing, const ring tlRing,
      bool assume = false, int indent = -1  );
  bool deBoGriPrint
    ( const uint* dvec, uint size, 
      const char* description, 
      uint flag, bool assume = false, int indent = -1 );
  bool deBoGriPrint
    ( const poly p, int shift, SD::kStrategy strat,
      const char* description, 
      uint flag, bool assume = false, int indent = -1 );
  bool loGriToFile
    ( const char* message, size_t number, uint flag,
      const void* addr = NULL, 
      const char* logfile = "logri.log", 
      bool assume = false, int indent = 0            );

  bool lmHasZeroShift(poly p, ring p_lm_ring, int lV);


#ifdef HAVE_DEBOGRITEST
  void deBoGriTTest(kStrategy strat);
#endif

  void memory_log( const char* descr,
                   const void* ptr, int line, const char* file );
  void* dAlloc(size_t size, int line, const char* file);
  void* dAlloc0(size_t size, int line, const char* file);
  void  dFreeSize( void* ptr, size_t size,
                   int line, const char* file );
  void* dReallocSize( void* ptr,
                      size_t size_old,
                      size_t size_new,
                      int line, const char* file );
  char* backtrace_string();
  poly dp_LPshiftT( poly p, int sh, kStrategy strat,
                    const ring r, int line, const char* file );
  poly dp_mLPshift( poly p, int sh, kStrategy strat,
                    const ring r, int line, const char* file );
  void dFreeSize( void* ptr, size_t size,
                   int line, const char* file );
  poly dp_Init( ring r, int line, const char* file );
  poly dp_Head( poly p, ring r,
                int line, const char* file );
  poly dp_Copy( poly p, ring rh, ring rt,
                int line, const char* file );
  void dp_LmFree( poly p, ring r,
                  int line, const char* file );
  void dp_LmFree( poly* p, ring r,
                  int line, const char* file );
  void dp_Delete( poly* p, ring r,
                  int line, const char* file );
  void dp_Delete( poly* p, ring rlm, ring rtl,
                  int line, const char* file );
  void dp_LmDelete( poly p, ring r,
                    int line, const char* file );

  typedef skStrategy* kStrategy;


#if DEBOGRI > 0

  //This variable will now hold the flags for triggering certain
  //debug output. (These are the flags formerly used for DEBOGRI; 
  //see above.) The DEBOGRI variable will become obsolete for
  //this purpose.
  extern int deBoGri;


  extern int indent;
  extern int indentInc;
  extern int isSilenced;
  extern int debugCounter;
#endif
};

#if DEBOGRI > 0
//Counts how many Objects of the deBoGri class are
//instantiated (useful for breakpoints in the code)
extern int ShiftDVec__deBoGriCnt;
#endif

class ShiftDVec::DeBoGri
{
  private:
    bool silenced;
    int flag;
    int& indent;
    const char* leaveMsg;

  public:
    DeBoGri(int& indent_);
    DeBoGri(int& indent_, const char* startMsg, uint flag);
    DeBoGri
      (int& indent_, const char* startMsg, const char* leaveMsg, uint flag);
    ~DeBoGri();

    void silence(){ ShiftDVec::isSilenced = silenced = true; }
    void speak(){ ShiftDVec::isSilenced = silenced = false; }
    void silenceIf(bool test)
      { ShiftDVec::isSilenced = silenced = test; }
    void speakIf(bool test)
      { ShiftDVec::isSilenced = silenced = !test; }
    bool isSilenced(){ return silenced; }
};

#define initDeBoGri(indentRef, startMsg, leaveMsg, flag) \
  ShiftDVec::DeBoGri debogriDbgObj(indentRef, startMsg, leaveMsg, flag)

#define deBoGriSilence() \
  debogriDbgObj.silence()

#define deBoGriSpeak() \
  debogriDbgObj.speak()

#define deBoGriSilenceIf(test) \
  debogriDbgObj.silenceIf(test)

#define deBoGriSpeakIf(test) \
  debogriDbgObj.speakIf(test)

#define deBoGriInitCounter() \
  static int counter = 0

#define deBoGriIncCounter() \
  ++counter

#define deBoGriPrintCounter(description, flag) \
  deBoGriPrint(counter, description, flag)

#define OMALLOC( size, ptype )\
  ShiftDVec::dAlloc( (size)*sizeof(ptype), __LINE__, __FILE__ )

#define OMALLOC_0( size, ptype )\
  ShiftDVec::dAlloc0( (size)*sizeof(ptype), __LINE__, __FILE__ )

#define OMFREES( ptr, size, ptype )\
  ShiftDVec::dFreeSize((void*)(ptr),\
                       (size)*sizeof(ptype), __LINE__, __FILE__)

#define OMREALLOCS( ptr, size_old, size_new, ptype )\
  ShiftDVec::dReallocSize( (void*)(ptr),\
                           (size_old)*sizeof(ptype),\
                           (size_new)*sizeof(ptype),\
                           __LINE__, __FILE__ )

#define MEMORY_LOG(descr, ptr)\
  ShiftDVec::memory_log(descr, ptr, __LINE__, __FILE__)
#define P_INIT(r)      dp_Init( r, __LINE__, __FILE__ )
#define P_HEAD(p, r)   dp_Head( p, r, __LINE__, __FILE__ )
#define P_COPY(p, rlm, rtl)\
  dp_Copy( p, rlm, rtl, __LINE__, __FILE__ )
#define P_DELETE(...)  dp_Delete(__VA_ARGS__, __LINE__, __FILE__)
#define P_LMFREE(p, r)   dp_LmFree( p, r, __LINE__, __FILE__ )
#define P_LMDELETE(p, r) dp_LmDelete( p, r, __LINE__, __FILE__ )
#define PMLP_SHIFT( p, sh, strat, r )\
  dp_mLPshift( p, sh, strat, r, __LINE__, __FILE__ )
#define PLP_SHIFT_T( p, sh, strat, r )\
  dp_LPshiftT( p, sh, strat, r, __LINE__, __FILE__ )

#ifndef HAVE_DEBOTRITEST
#define deBoGriTTest(x1) ((void) 0)
#endif

#else //#if DEBOGRI > 0 i.e DEBOGRI <= 0

#define ExtensionTTest(...) ((void) 0)
#define DegreeTTest(...) ((void) 0)
#define deBoGriTTest(x1) ((void) 0)
#define loGriToFile(...) ((void) 0)
#define initDeBoGri(...) ((void) 0)
#define deBoGriPrint(...) ((void) 0)
#define deBoGriSpeak() ((void) 0)
#define deBoGriSilence() ((void) 0)
#define deBoGriSpeakIf(test) ((void) 0)
#define deBoGriSilenceIf(test) ((void) 0)
#define deBoGriInitCounter() ((void) 0)
#define deBoGriPrintCounter(x2, x3) ((void) 0)
#define deBoGriIncCounter() ((void) 0)
#define lmHasZeroShift(...) ((void) 0)

#define OMALLOC( size, ptype )   omAlloc( (size)*sizeof(ptype) )
#define OMALLOC_0( size, ptype ) omAlloc0( (size)*sizeof(ptype) )
#define OMFREES( ptr, size, ptype )\
  omFreeSize( (void*)(ptr), (size)*sizeof(ptype) )
#define OMREALLOCS( ptr, size_old, size_new, ptype )\
  omReallocSize( ptr,\
                 (size_old)*sizeof(ptype),\
                 (size_new)*sizeof(ptype)   )

#define MEMORY_LOG(...) ((void) 0)
#define P_INIT(r)              p_Init(r)
#define P_HEAD(p, r)           p_Head(p, r)
#define P_COPY(p, rlm, rtl)    p_Copy(p, rlm, rtl)
#define P_DELETE(...)          p_Delete(__VA_ARGS__)
#define P_LMFREE(p, r)         p_LmFree(p, r)
#define P_LMDELETE(p, r)       p_LmDelete(p, r)
#define PMLP_SHIFT( p, sh, strat, r )\
  ::p_mLPshift( p, sh, strat->get_uptodeg(), strat->get_lV(), r )
#define PLP_SHIFT_T( p, sh, strat, r )\
  p_LPshiftT( p, sh,\
              strat->get_uptodeg(), strat->get_lV(), strat, r )

#endif //#if DEBOGRI > 0


#endif //ifndef SDDEBUG_H


// vim: set foldmethod=syntax :
// vim: set textwidth=65 :
// vim: set colorcolumn=+1 :
