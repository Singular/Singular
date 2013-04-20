#ifndef SDDEBUG_H
#define SDDEBUG_H

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
  class DeBoGri;

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
    ( const poly p, const char* description, uint flag,
      const ring lmRing, const ring tlRing = NULL,
      bool assume = false, int indent = -1              );
  bool deBoGriPrint
    ( const TObject* P, const char* description, uint flag,
      const ring lmRing, const ring tlRing,
      bool assume = false, int indent = -1                 );
  bool deBoGriPrint
    ( const uint* dvec, uint size, 
      const char* description, 
      uint flag, bool assume = false, int indent = -1 );
  bool deBoGriPrint
    ( const poly p, int shift, kStrategy strat,
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

#ifndef HAVE_DEBOTRITEST
#define deBoGriTTest(x1) ((void) 0)
#endif

#else //#if DEBOGRI > 0 i.e DEBOGRI <= 0

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

#endif //#if DEBOGRI > 0


#endif //ifndef SDDEBUG_H


/* vim: set foldmethod=syntax : */
