/* Preamble
 * Definitions for the letterplace DVec implementations
 * Last Change: 2012-03-20 15:00
 * Maintainers: Grischa Studzinski and Benjamin Schnitzler 
 */

/* TODO:
 * a lot
 */

#ifndef KUTIL2_H
#define KUTIL2_H

#define HAVE_SHIFTBBADVEC

//BOCO: Our personal debugging output
//
// Attention:
// The usage of DEBOGRI as flag-variable became obsolete.
// ShiftDVec::deBoGri is the successor.
//
//0 : RECOMMENDED for not-developers
//    no debugging output; removes all in DEBOGRI defined debug
//    functions from the code, which is recommended, if you do
//    not want to debug the code.
//1 : entering of pairs - check degree violation
//2 : Which pairs are sorted out by... some criterion?
//4 : debugging of Gebauer-Moeller
//8 : debugging of the initialization of LObjects
//16 : debugging of lcm related things
//32 : debugging of missing pairs
//64 : debugging of chainCrit
//128 : Which pairs are sorted out by some criterion?
//      - at the moment, this considers only GM3.
//256 : debugging divisiblility/ reduction related things
//512 : debugging Creation of DVecs
//1024 : Debugging memory corruption
//2048 : debugging redTail related things
#ifdef KDEBUG
#define DEBOGRI 1
#endif
//TODO: 
// - Introduce a singular option for setting debogri whilst in
//   debug mode.
// - Create a function, which can turn on/off debug output for
//   all debogris, local to the current function (works together
//   with initdebogri?)


//BOCO: This was an idea, but it is cancelled at the moment
//#define USE_DVEC_LCM


#include <kernel/mod2.h> //Needed because of include order

namespace ShiftDVec
{ uint CreateDVec(poly p, ring r, uint*& dvec); }

#include <kernel/kutil.h>

typedef unsigned int uint;


namespace ShiftDVec{
  class sLObject;
  class sTObject;

  typedef class ShiftDVec::sLObject LObject;
  typedef class ShiftDVec::sTObject TObject;

  uint getShift
    (poly p, uint numFirstVar, ring r = currRing );

  bool compareDVec
      ( const uint* dvec, poly p, 
        uint offset, uint maxSize, ring r = currRing );

  uint divisibleBy
    ( const uint* dvec1, uint dvSize1, 
      const uint* dvec2, uint dvSize2, int numVars );

  uint divisibleBy
    ( const uint* dvec1, uint dvSize1, 
      const uint* dvec2, uint dvSize2, 
      uint minShift, uint maxShift, int numVars );

  uint divisibleBy
    ( ShiftDVec::sTObject * t1, 
      ShiftDVec::sTObject * t2, int numVars );

  uint findRightOverlaps
    ( ShiftDVec::sTObject * t1, ShiftDVec::sTObject * t2, 
      int numVars, int maxDeg, uint ** overlaps );

  BOOLEAN redViolatesDeg
    ( poly a, poly b, int uptodeg,
      ring aLmRing = currRing, 
      ring bLmRing = currRing, ring bTailRing = currRing );
  BOOLEAN redViolatesDeg
    ( TObject* a, TObject* b, 
      int uptodeg, ring lmRing = currRing );

  bool createSPviolatesDeg
    ( poly a, poly b, uint shift, int uptodeg, 
      ring aLmRing = currRing, ring bLmRing = currRing, 
      ring aTailRing = currRing, ring bTailRing = currRing );
  bool shiftViolatesDeg
    ( poly p, uint shift, int uptodeg, 
      ring pLmRing = currRing, ring pTailRing = currRing );

  void dvecWrite(const poly p, ring r = currRing);

  void dvecWrite(const uint* dvec, uint dvSize);

  void dvecWrite(const sTObject* t);

  void lcmDvecWrite(const sLObject* t);

  extern int lpDVCase; //defined in kutil2.cc
};


#if DEBOGRI > 0

#if 0
//This has to be changed to make it work again
//void debugPrintDegViolation
//  (const char* where, poly p1, uint shift, poly p2, kStrategy);
#define debugPrintDegViolation(x1, x2, x3, x4, x5) ((void) 0)
#else
#define debugPrintDegViolation(x1, x2, x3, x4, x5) ((void) 0)
#endif

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

#endif //#if DEBOGRI > 0



class sTObject;

class ShiftDVec::sTObject : public ::sTObject
{
public:
  uint * dvec; //Distance Vector of lm(p)
  uint dvSize; //size of the >>uint * dvec<< array
  //uint pIsInR; //already i_r in ::sTObject

  // constructors
  sTObject(ring r = currRing) : 
    ::sTObject(r), dvec(NULL){ t_p = NULL; }

  sTObject(poly p, ring tailRing = currRing) : 
    ::sTObject(p, tailRing),  dvec(NULL){ t_p = NULL; }

  sTObject(poly p, ring c_r, ring tailRing) : 
    ::sTObject(p, c_r, tailRing),  dvec(NULL){ t_p = NULL; }

  sTObject(sTObject* T, int copy) : 
    ::sTObject(T, copy),  dvec(NULL){ t_p = NULL; }

#if 0
  //destructor - 
  //freeing dvecs automatically was to problematic with singular
  ~sTObject()
  { this->freeDVec(); }
#endif

  void dumbInit(poly p_){ p = p_; dvec = NULL; t_p = NULL; }

  void SetDVecIfNULL(poly p, ring r)
  { if(!dvec) SetDVec(p, r); }

  void SetDVecIfNULL()
  { if(!dvec) SetDVec(); }
  
  // Initialize the distance vector of an ShiftDVec::sTObject
  void SetDVec(poly p, ring r)
  { 
    freeDVec(); 
    dvSize = CreateDVec(p, r, dvec); 
  }

  // Initialize the distance vector of an ShiftDVec::sTObject
  void SetDVec()
  { 
    freeDVec(); 
    if(p == NULL)
      dvSize = CreateDVec(t_p, tailRing, dvec); 
    else
      dvSize = CreateDVec(p, currRing, dvec); 
  }

  void SetDVec(uint* dv) {dvec = dv;}

  uint*  GetDVec(); 
  uint  getDVecAtIndex(uint index) {return dvec[index];}
  uint GetDVsize(); 

  int cmpDVec(ShiftDVec::sTObject* toCompare);

  int cmpDVec
    (ShiftDVec::sTObject* T1, uint begin, uint beginT1, uint size);

  int cmpDVecProper
    ( ShiftDVec::sTObject* T1, 
      uint begin, uint beginT1, uint size, int lV );

  void freeDVec(); 

  // may need adjustments
  ShiftDVec::sTObject& operator=
    (const ShiftDVec::sTObject& t);

  uint divisibleBy( ShiftDVec::sTObject * T, 
                    int numVars                    )
  { SetDVecIfNULL(); T->SetDVecIfNULL();
    return ShiftDVec::divisibleBy
      (dvec, dvSize, T->dvec, T->dvSize, numVars); }
};

#endif  //#ifndef KUTIL2_H
