#include <SDDebug.h>
/* file:        SDDebug.cc
 * authors:     Grischa Studzinski & Benjamin Schnitzler
 * created:     ask git
 * last change: ask git
 *
 * ELSE:
 * see SDDebug.h
 */

typedef skStrategy* kStrategy;

namespace SD = ShiftDVec;

void ShiftDVec::dvecWrite(const poly p, ring r)
{
  uint dvSize = p_Totaldegree(p, r);
  for(int j=1, i=1, l=0; l < dvSize; ++i)
    if(p_GetExp(p,i,r)){Print(" %d ",j);j=1;++l;} else{++j;}
}


void ShiftDVec::dvecWrite(const uint* dvec, uint dvSize)
{
  PrintS("( ");
  for(int i = 0; i < dvSize; ++i){ Print(" %d ", dvec[i]); }
  PrintS(" )");
}


void ShiftDVec::dvecWrite(const SD::TObject* t)
{ dvecWrite(t->dvec, t->dvSize); }


void ShiftDVec::lcmDvecWrite(const SD::LObject* t)
{ dvecWrite(t->lcmDvec, t->lcmDvSize); }


int SD::lpDVCase = 0;



//class deBoGriInitializer functions



#if DEBOGRI > 0
int ShiftDVec__deBoGriCnt = 0;
int SD::deBoGri = 0;
int SD::indent = 0;
int SD::indentInc = 2;
int SD::isSilenced = false;
int SD::debugCounter = 0;
#endif

#if DEBOGRI > 0
SD::DeBoGri::DeBoGri
  ( int& indent_ ) : indent(indent_), leaveMsg(NULL)
{ SD::indent = indent += SD::indentInc; }
#endif

#if DEBOGRI > 0
SD::DeBoGri::DeBoGri
  ( int& indent_, const char* startMsg, uint flag_ ) : 
  indent(indent_), leaveMsg(NULL), flag(flag_)
{
  deBoGriPrint( startMsg, flag_ );
  SD::indent = indent += SD::indentInc;
}
#endif

#if DEBOGRI > 0
SD::DeBoGri::DeBoGri
  ( int& indent_, 
    const char* startMsg, const char* leaveMsg_, uint flag_ ) : 
  indent(indent_), leaveMsg(leaveMsg_), flag(flag_)
{
  ShiftDVec__deBoGriCnt++;
  deBoGriPrint( startMsg, flag_ );
  SD::indent = indent += SD::indentInc;
}
#endif

#if DEBOGRI > 0
SD::DeBoGri::~DeBoGri() 
{
  ShiftDVec__deBoGriCnt++;
  SD::indent = indent - SD::indentInc; 
  deBoGriPrint( leaveMsg, flag );
}
#endif



//debogri debug functions



#if 0  //This has to be changed to make it work again
/* Test if degree violation checks works as expected. This
 * function will do nothing, if DEBOGRI & 1 == 0 .         */
void ShiftDVec::debugPrintDegViolation
  ( const char* where, poly p1, uint shift, 
    poly p2, SD::kStrategy strat                )
{
  namespace SD = ShiftDVec;
  PrintS("\nAt "); Print(where); PrintLn();
  if( shiftViolatesDeg(p1, shift, strat->uptodeg) )
  {
    PrintS("Shift of polynomial\n" );
    pWrite(p1);
    PrintS("would violate the degree.\n");
    Print("Shift: %d\n", shift);
    PrintLn();
  } else if( SD::createSPviolatesDeg
               (p1, p2, shift, strat->uptodeg) )
  {
    poly pTemp = p_LPshift
      ( p1, shift, strat->uptodeg, strat->lV, currRing );

    PrintS("Creation of s-poly would violate the degree:\n");
    PrintS("Poly 1:"); pWrite(p2);
    PrintS("Poly 2:"); pWrite(pTemp);
    PrintLn();

    pDelete(&pTemp);
  } else {
    PrintS("No degree violation\n");
    poly pTemp = p_LPshift
      ( p1, shift, strat->uptodeg, strat->lV, currRing );
    deBoGriPrint("Not shiftet Poly in pair: ", p2, 1);
    deBoGriPrint("Shifted Poly in pair: ", pTemp, 1);
  }
}
#endif

#if DEBOGRI > 0
bool ShiftDVec::lmHasZeroShift(poly p, ring p_lm_ring, int lV)
{
  for(int i = 1; i <= lV; ++i)
    if( p_GetExp(p,i,p_lm_ring) ) return true;

  return false;
}
#endif

#if DEBOGRI > 0
bool ShiftDVec::deBoGriPrint
  ( const char* description, uint flag, bool assume, int indent )
{
  if(indent < 0){ indent = SD::indent; }
  if( SD::deBoGri & flag && 
      !assume && !SD::isSilenced )
  { 
    for(int i = indent; i; --i) PrintS(" ");
    PrintS(description); PrintLn();
  }
  return assume;
}
#endif

#if DEBOGRI > 0
bool ShiftDVec::deBoGriPrint
  ( const poly p, 
    const char* description, uint flag, bool assume, int indent )
{
  if(indent < 0){ indent = SD::indent; }
  if( SD::deBoGri & flag && 
      !assume && !SD::isSilenced )
  {
    for(int i = indent; i; --i) PrintS(" ");
    PrintS(description); pWrite(p);
  }
  return assume;
}
#endif

#if DEBOGRI > 0
bool ShiftDVec::deBoGriPrint
  ( const poly p, const char* description, uint flag, 
    const ring lmRing, const ring tlRing, 
    bool assume, int indent )
{
  if(indent < 0){ indent = SD::indent; }
  if( SD::deBoGri & flag && 
      !assume && !SD::isSilenced )
  {
    for(int i = indent; i; --i) PrintS(" ");
    PrintS(description); 
    if(tlRing == NULL) p_Write(p, lmRing, lmRing);
    else p_Write(p, lmRing, tlRing);
  }
  return assume;
}
#endif

#if DEBOGRI > 0
bool ShiftDVec::deBoGriPrint
  ( const SD::TObject* P, const char* description, uint flag, 
    const ring lmRing, const ring tlRing, 
    bool assume, int indent )
{
  if(indent < 0){ indent = SD::indent; }
  if( SD::deBoGri & flag && 
      !assume && !SD::isSilenced )
  {
    for(int i = indent; i; --i) PrintS(" ");
    PrintS(description); 
    if(P->p != NULL){ p_Write(P->p, lmRing, tlRing); }
    else { p_Write(P->t_p, tlRing, tlRing ); }
  }
  return assume;
}
#endif

#if DEBOGRI > 0
bool SD::deBoGriPrint
  ( const poly p, int shift, SD::kStrategy strat,
    const char* description, uint flag, bool assume, int indent )
{
  if(indent < 0){ indent = SD::indent; }
  if( SD::deBoGri & flag && 
      !assume && !SD::isSilenced )
  {
    poly pTemp = p;
    if( shiftViolatesDeg(p, shift, strat->uptodeg) )
    {
      PrintS("DEBOGRIPRINT: Shift would violate degree bound.\n"); 
      PrintS("DEBOGRIPRINT: Will print the polynomial unshifted.\n"); 
    }
    else if(shift != 0)
     {
      pTemp = p_LPshift
        ( p, shift, strat->uptodeg, strat->lV, currRing );
      loGriToFile("p_LPshift in deBoGriPrint",0 ,1024, (void*)pTemp);
     }
    for(int i = indent; i; --i) PrintS(" ");
    PrintS(description); pWrite(pTemp);

    if(p != pTemp)
    { 
     loGriToFile("pDelete in deBoGriPrint",0 ,1024, (void*)pTemp);
     pDelete(&pTemp);
    }
  }
  return assume;
}
#endif

#if DEBOGRI > 0
bool SD::deBoGriPrint
  ( const uint* dvec, uint size, 
    const char* description, uint flag, bool assume, int indent )
{
  if(indent < 0){ indent = SD::indent; }
  if( SD::deBoGri & flag && 
      !assume && !SD::isSilenced )
  {
    for(int i = indent; i; --i) PrintS(" ");
    PrintS(description); dvecWrite(dvec, size); PrintLn();
  }
  return assume;
}
#endif

#if DEBOGRI > 0
bool ShiftDVec::deBoGriPrint
  ( int number, 
    const char* description, uint flag, bool assume, int indent )
{
  if(indent < 0){ indent = SD::indent; }
  if( SD::deBoGri & flag && 
      !assume && !SD::isSilenced )
  {
    for(int i = indent; i; --i) PrintS(" ");
    PrintS(description); Print("%d\n", number);
  }
  return assume;
}
#endif

#if DEBOGRI > 0
bool ShiftDVec::deBoGriPrint
  ( void* address, 
    const char* description, uint flag, bool assume, int indent )
{
  if(indent < 0){ indent = SD::indent; }
  if( SD::deBoGri & flag && 
      !assume && !SD::isSilenced )
  {
    for(int i = indent; i; --i) PrintS(" ");
    PrintS(description); Print("%p\n", address);
  }
  return assume;
}
#endif

#if DEBOGRI > 0
bool ShiftDVec::loGriToFile
  ( const char* message, size_t number, uint flag,
    const void* addr, 
    const char* logfile, bool assume, int indent   )
{
  if(indent < 0){ indent = SD::indent; }
  if( SD::deBoGri & flag 
      && !assume && !SD::isSilenced )
  {
#if 0 //Replaced
    std::ofstream toFile;
    toFile.open(logfile, std::ios::out | std::ios::app);
    for(int i = indent; i; --i) toFile << " ";
    toFile << message << number;
    if(addr){ toFile << " Adresse: " << addr << "\n";}
    else    { toFile << "\n"; }
    toFile.close();
#else //Replacement
    //TODO: addr als Adresse printen
    FILE* log = fopen(logfile, "a");
    for(int i = indent; i; --i) fprintf(log, " ");
    fprintf(log, "%s%d Adresse: %p\n", message, number, addr);
    fclose(log);
#endif
  }
  return assume;
}
#endif

#if DEBOGRI > 0 && HAVE_DEBOGRITEST
void ShiftDVec::deBoGriTTest(kStrategy strat)
{
  if (strat->T != NULL)
  {
    for (int i=0; i<=strat->tl; i++)
    {
      kTest_T(&(strat->T[i]), strat->tailRing, i, 'T');
    }
  }
}
#endif

/* vim: set foldmethod=syntax : */
