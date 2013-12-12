#include <kernel/SDDebug.h>
/* file:        SDDebug.cc
 * authors:     Grischa Studzinski & Benjamin Schnitzler
 * created:     ask git
 * last change: ask git
 *
 * ELSE:
 * see SDDebug.h
 */

//#include <kernel/SDkutil.h>//do this via kutil.h otherwise...
#include <kutil.h>

#if DEBOGRI > 0
#include <execinfo.h>  // for backtrace procedures (linux only)
#endif

namespace SD = ShiftDVec;

#if DEBOGRI > 0
/** Test if SD Extentions are initialized correct
 *
 * \remark
 *   Following the a strange Singular tradition, tl is equal to
 *   the number of TObjects in T plus one! If tl == -2, then
 *   T is assumed to be a pointer to a single TObject.
 */
void ShiftDVec::ExtensionTTest(sTObject* T, int tl)
{
  if(tl == -1 || tl < -2) return;
  if(tl == -2)
    if(T->SD_Ext()) assume(T->SD_Ext()->T == T);
  else
    for(int i = 0; i <= tl; ++i)
      if(T[i].SD_Ext()) assume(T[i].SD_Ext()->T == T+i);
}
#endif

#if DEBOGRI > 0
/** Test if T->FDeg is initialized correctly
 *
 * \remark
 *   Following the a strange Singular tradition, tl is equal to
 *   the number of TObjects in T plus one! If tl == -2, then
 *   T is assumed to be a pointer to a single TObject.
 */
void ShiftDVec::DegreeTTest(sTObject* T, int tl)
{
  if(tl == -1 || tl < -2) return;
  if(tl == -2)            assume( T->FDeg == T->pFDeg() );
  else
    for(int i = 0; i <= tl; ++i)
      assume( T[i].FDeg == T[i].pFDeg() );
}
#endif


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


void ShiftDVec::dvecWrite(const TObject* t)
{ dvecWrite(t->SD_Ext()->dvec, t->SD_Ext()->dvSize); }


void ShiftDVec::lcmDvecWrite(LObject* t)
{
  dvecWrite( t->SD_Ext()->GetDVec(),
             t->SD_LExt()->getLcmDVSize() );
}


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
  ( const TObject* P, const char* description, uint flag, 
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
    if( shiftViolatesDeg(p, shift, strat->get_uptodeg()) )
    {
      PrintS("DEBOGRIPRINT: Shift would violate degree bound.\n"); 
      PrintS("DEBOGRIPRINT: Will print the polynomial unshifted.\n"); 
    }
    else if(shift != 0)
     {
      pTemp = p_LPshift
        ( p, shift,
          strat->get_uptodeg(), strat->get_lV(), currRing );
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


#if DEBOGRI > 0
char* ShiftDVec::backtrace_string()
{
  const int bt_buf_size = 32;
  const int bt_str_buf_size = 1024;

  void* bt_buffer[bt_buf_size];
  static char ret_str[bt_str_buf_size];
  strcpy(ret_str, "    ");

  int act_depth = backtrace(bt_buffer, bt_buf_size);
  char** bt_symbols = backtrace_symbols(bt_buffer, act_depth);

  for(int i = 0; i < act_depth; ++i)
  {
    if( strchr(bt_symbols[i], '+') )
    {
      char* first = strchr(bt_symbols[i], '(');
      ++first;
      if( i > 0 ) strncat(ret_str, "\n    ", bt_str_buf_size);
      strncat(ret_str, strtok(first, "+"), bt_str_buf_size);
    }
  }

  return ret_str;
}

void ShiftDVec::memory_log( const char* descr,
                            const void* ptr,
                            int line, const char* file )
{
  static FILE* fd_mem_log = NULL;

  if(!fd_mem_log) fd_mem_log = fopen("SDMemory.log", "w");


  fprintf( fd_mem_log, "%-15s %-15p", descr, ptr );
  fprintf( fd_mem_log, "  %4d:%-22s\n", line, file );

  char* bt = backtrace_string();
  fprintf( fd_mem_log, "%s\n", bt );

  fflush( fd_mem_log );
}

void* ShiftDVec::dAlloc(size_t size, int line, const char* file)
{
  void* ptr = omAlloc(size);
  memory_log( "omAlloc", ptr, line, file );
  return ptr;
}

void* ShiftDVec::dAlloc0( size_t size, int line, const char* file)
{
  void* ptr = omAlloc0(size);
  memory_log( "omAlloc0", ptr, line, file );
  return ptr;
}

void* ShiftDVec::dReallocSize( void* ptr,
                               size_t size_old,
                               size_t size_new,
                               int line, const char* file )
{
  memory_log( "omReallocSize", ptr, line, file );
  ptr = omReallocSize( ptr, size_old, size_new );
  memory_log( "omReallocSize", ptr, line, file );
  return ptr;
}

void ShiftDVec::dFreeSize( void* ptr, size_t size,
                            int line, const char* file )
{

  memory_log( "omFreeSize", ptr, line, file );
  omFreeSize( ptr, size );
}

poly ShiftDVec::dp_Init( ring r, int line, const char* file )
{
  poly p = p_Init(r);
  memory_log( "p_Init", p, line, file );
  return p;
}

poly ShiftDVec::dp_Head( poly p, ring r,
                         int line, const char* file )
{
  p = p_Head(p, r);
  memory_log( "p_Head", p, line, file );
  return p;
}

poly ShiftDVec::dp_Copy( poly p, ring rh, ring rt,
                         int line, const char* file )
{
  p = p_Copy(p, rh, rt);
  memory_log( "p_Copy", p, line, file );
  return p;
}

void ShiftDVec::dp_LmFree( poly p, ring r,
                           int line, const char* file )
{
  memory_log( "p_LmFree", p, line, file );
  p_LmFree(p, r);
}

void ShiftDVec::dp_LmFree( poly* p, ring r,
                           int line, const char* file )
{
  memory_log( "p_LmFree", *p, line, file );
  p_LmFree(p, r);
}

void ShiftDVec::dp_Delete( poly* p, ring r,
                           int line, const char* file )
{
  memory_log( "p_Delete", *p, line, file );
  p_Delete(p, r);
}

void ShiftDVec::dp_Delete( poly* p, ring rlm, ring rtl,
                           int line, const char* file )
{
  memory_log( "p_Delete", *p, line, file );
  p_Delete(p, rlm, rtl);
}

void ShiftDVec::dp_LmDelete( poly p, ring r,
                             int line, const char* file )
{
  memory_log( "p_LmDelete", p, line, file );
  p_LmDelete(p, r);
}

poly ShiftDVec::dp_LPshiftT( poly p, int sh,
                             kStrategy strat, const ring r,
                             int line, const char* file )
{
  p = p_LPshiftT( p, sh,
                  strat->get_uptodeg(),
                  strat->get_lV(), strat, r );
  memory_log( "dp_LPshiftT", p, line, file );
  return p;
}

poly ShiftDVec::dp_mLPshift( poly p, int sh,
                             kStrategy strat, const ring r,
                             int line, const char* file )
{
  p = p_mLPshift( p, sh,
                  strat->get_uptodeg(), strat->get_lV(), r );
  memory_log( "dp_mLPshift", p, line, file );
  return p;
}
#endif

// vim: set foldmethod=syntax :
// vim: set textwidth=65 :
// vim: set colorcolumn=+1 :
