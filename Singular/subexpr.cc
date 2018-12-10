/*!
!
***************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*!
!

* ABSTRACT: handling of leftv


*/

#include "kernel/mod2.h"

#ifdef HAVE_OMALLOC
#include "omalloc/omalloc.h"
#else
#include "xalloc/omalloc.h"
#endif

#include "misc/intvec.h"
#include "misc/options.h"

#include "coeffs/numbers.h"
#include "coeffs/bigintmat.h"

#include "coeffs/ffields.h" // nfShowMipo // minpoly printing...

#include "polys/monomials/maps.h"
#include "polys/matpol.h"
#include "polys/monomials/ring.h"

// #include "coeffs/longrat.h"

#include "kernel/polys.h"
#include "kernel/ideals.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/syz.h"
#include "kernel/oswrapper/timer.h"

#include "Singular/tok.h"
#include "Singular/ipid.h"
#include "Singular/ipshell.h"
#include "Singular/lists.h"
#include "Singular/attrib.h"
#include "Singular/links/silink.h"
#include "Singular/attrib.h"
#include "Singular/ipprint.h"
#include "Singular/subexpr.h"
#include "Singular/blackbox.h"
#include "Singular/number2.h"

#include <ctype.h>

omBin sSubexpr_bin = omGetSpecBin(sizeof(_ssubexpr));
omBin sleftv_bin = omGetSpecBin(sizeof(sleftv));
omBin procinfo_bin = omGetSpecBin(sizeof(procinfo));
omBin libstack_bin = omGetSpecBin(sizeof(libstack));
static omBin size_two_bin = omGetSpecBin(2);

sleftv     sLastPrinted;
#ifdef SIQ
BOOLEAN siq=FALSE;
#endif

int sleftv::listLength()
{
  int n = 1;
  leftv sl = next;
  while (sl!=NULL)
  {
    n++;
    sl=sl->next;
  }
  return n;
}

void sleftv::Print(leftv store, int spaces)
{
  int  t=Typ();
  if (errorreported) return;
#ifdef SIQ
  if (rtyp==COMMAND)
  {
    command c=(command)data;
    char ch[2];
    ch[0]=c->op;ch[1]='\0';
    const char *s=ch;
    if (c->op>127) s=iiTwoOps(c->op);
    ::Print("##command %d(%s), %d args\n",
      c->op, s, c->argc);
    if (c->argc>0)
      c->arg1.Print(NULL,spaces+2);
    if(c->argc<4)
    {
      if (c->argc>1)
        c->arg2.Print(NULL,spaces+2);
      if (c->argc>2)
        c->arg3.Print(NULL,spaces+2);
    }
    PrintS("##end");
  }
  else
#endif
  {
    const char *n=Name();
    char *s;
    void *d=Data();
    if (errorreported) return;

    switch (t /*!
!
=Typ()

*/)
      {
        case CRING_CMD:
          crPrint((coeffs)d);
          break;
#ifdef SINGULAR_4_2
        case CNUMBER_CMD:
          n2Print((number2)d);
          break;
        case CPOLY_CMD:
          p2Print((poly2)d);
          break;
        case CMATRIX_CMD: // like BIGINTMAT
#endif
        case BIGINTMAT_CMD:
          ((bigintmat *)d)->pprint(colmax);
          break;
        case BUCKET_CMD:
          {
            sBucket_pt b=(sBucket_pt)d;
            if ((e==NULL)
            && (TEST_V_QRING)
            &&(currRing->qideal!=NULL))
            {
              poly p=pCopy(sBucketPeek(b));
              jjNormalizeQRingP(p);
              PrintNSpaces(spaces);
              pWrite0(p);
              pDelete(&p);
              break;
            }
            else
              sBucketPrint(b);
          }
          break;
        case UNKNOWN:
        case DEF_CMD:
          PrintNSpaces(spaces);
          PrintS("`");PrintS(n);PrintS("`");
          break;
        case PACKAGE_CMD:
          PrintNSpaces(spaces);
          paPrint(n,(package)d);
          break;
        case LIB_CMD:
        case NONE:
          return;
        case INTVEC_CMD:
        case INTMAT_CMD:
          ((intvec *)d)->show(t,spaces);
          break;
        case RING_CMD:
        {
          PrintNSpaces(spaces);
          const ring r = (const ring)d;
          rWrite(r, currRing == r);
          break;
        }
        case MATRIX_CMD:
          iiWriteMatrix((matrix)d,n,2, currRing, spaces);
          break;
        case SMATRIX_CMD:
        {
          matrix m = id_Module2Matrix(id_Copy((ideal)d,currRing),currRing);
          ipPrint_MA0(m, n);
          id_Delete((ideal *) &m,currRing);
          break;
        }
        case MODUL_CMD:
        case IDEAL_CMD:
          if ((TEST_V_QRING)  &&(currRing->qideal!=NULL)
          &&(!hasFlag(this,FLAG_QRING)))
          {
            jjNormalizeQRingId(this);
            d=Data();
          }
          // no break:
        case MAP_CMD:
          iiWriteMatrix((matrix)d,n,1, currRing, spaces);
          break;
        case POLY_CMD:
        case VECTOR_CMD:
          if ((e==NULL)
          && (TEST_V_QRING)
          &&(currRing->qideal!=NULL)
          &&(!hasFlag(this,FLAG_QRING)))
          {
            setFlag(this,FLAG_QRING);
            poly p=(poly)d;
            jjNormalizeQRingP(p);
            if (p!=(poly)d)
            {
              d=(void*)p;
              if ((rtyp==POLY_CMD)||(rtyp==VECTOR_CMD)) data=d;
              else if (rtyp==IDHDL)
              {
                idhdl h=(idhdl)data;
                IDPOLY(h)=p;
                setFlag(h,FLAG_QRING);
              }
            }
          }
          PrintNSpaces(spaces);
          pWrite0((poly)d);
          break;
        case RESOLUTION_CMD:
        {
          syStrategy tmp=(syStrategy)d;
          syPrint(tmp,IDID(currRingHdl));
          break;
        }
        case STRING_CMD:
          PrintNSpaces(spaces);
          PrintS((char *)d);
          break;
       case INT_CMD:
          PrintNSpaces(spaces);
          ::Print("%d",(int)(long)d);
          break;
       case PROC_CMD:
         {
           procinfov pi=(procinfov)d;

           PrintNSpaces(spaces);
           PrintS("// libname  : ");
           PrintS(piProcinfo(pi, "libname"));
           PrintLn();

           PrintNSpaces(spaces);
           PrintS("// procname : ");
           PrintS(piProcinfo(pi, "procname"));
           PrintLn();

           PrintNSpaces(spaces);
           PrintS("// type     : ");
           PrintS(piProcinfo(pi, "type"));
           //           ::Print("%-*.*s// ref      : %s",spaces,spaces," ",
           //   piProcinfo(pi, "ref"));
           break;
         }
       case LINK_CMD:
          {
            si_link l=(si_link)d;
            PrintNSpaces(spaces);
            ::Print("// type : %s\n", slStatus(l, "type"));
            PrintNSpaces(spaces);
            ::Print("// mode : %s\n", slStatus(l, "mode"));
            PrintNSpaces(spaces);
            ::Print("// name : %s\n", slStatus(l, "name"));
            PrintNSpaces(spaces);
            ::Print("// open : %s\n", slStatus(l, "open"));
            PrintNSpaces(spaces);
            ::Print("// read : %s\n", slStatus(l, "read"));
            PrintNSpaces(spaces);
            ::Print("// write: %s", slStatus(l, "write"));
          break;
          }
        case BIGINT_CMD:
          s=String(d);
          if (s==NULL) return;
          PrintNSpaces(spaces);
          PrintS(s);
          omFree((ADDRESS)s);
          break;
        case NUMBER_CMD:
          {
            number n=(number)d;
            nNormalize(n);
            if ((number)d !=n)
            {
              d=n;
              if (rtyp==IDHDL) IDNUMBER(((idhdl)data))=n;
              else if(rtyp==NUMBER_CMD) data=(void*)n;
            }
            s=String(d);
            if (s==NULL) return;
            PrintS(s);
            omFree((ADDRESS)s);
            break;
          }
        case LIST_CMD:
        {
          lists l=(lists)d;
          if (lSize(l)<0)
          {
             PrintNSpaces(spaces);
             PrintS("empty list\n");
          }
          else
          {
            int i=0;
            for (;i<=l->nr;i++)
            {
              if (l->m[i].rtyp!=DEF_CMD)
              {
                PrintNSpaces(spaces);
                ::Print("[%d]:\n",i+1);
                l->m[i].Print(NULL,spaces+3);
              }
            }
          }
          break;
        }

        default:
          if (t>MAX_TOK)
          {
            blackbox * bb=getBlackboxStuff(t);
            PrintNSpaces(spaces);
            if (bb!=NULL) { bb->blackbox_Print(bb,d); }
            else          { ::Print("Print: blackbox %d(bb=NULL)",t); }
          }
          else
          ::Print("Print:unknown type %s(%d)", Tok2Cmdname(t),t);
      } /*!
!
 end switch: (Typ()) 

*/
    if ((store!=NULL)&&(store!=this))
      store->CleanUp();
  }
  if (next!=NULL)
  {
    if (t==COMMAND) PrintLn();
    else if (t!=LIST_CMD) PrintS(" ");
    next->Print(NULL,spaces);
  }
  else if ((t!=LIST_CMD)&&(t!=SMATRIX_CMD))
  {
    PrintLn();
  }
#ifdef SIQ
  if (rtyp!=COMMAND)
#endif
  {
    if ((store!=NULL)
    && (store!=this))
    {
      if((t/*!
!
Typ()

*/!=LINK_CMD)
      && (t/*!
!
Typ()

*/!=PACKAGE_CMD)
      && (t/*!
!
Typ()

*/!=DEF_CMD)
      )
      {
        store->rtyp=t/*!
!
Typ()

*/;
        store->data=CopyD();
        if(attribute!=NULL)
        {
          store->attribute=CopyA();
        }
        store->flag=flag;
      }
    }
  }
}

void sleftv::CleanUp(ring r)
{
  if (rtyp!=IDHDL)
  {
    if ((name!=NULL) && (name!=sNoName_fe) && (rtyp!=ALIAS_CMD))
    {
      //::Print("free %x (%s)\n",name,name);
      omFree((ADDRESS)name); // may be larger >1000 char (large int)
    }
    //name=NULL;
    //flag=0;
    if (data!=NULL)
    {
      //if (rtyp==IDHDL) attribute=NULL; // is only a pointer to attribute of id
      s_internalDelete(rtyp,data,r);
      //data=NULL; // will be done by Init() at the end
    }
    if (attribute!=NULL)
    {
      switch (rtyp)
      {
        case PACKAGE_CMD:
        //case IDHDL:
        case ANY_TYPE:
        case VECHO:
        case VPRINTLEVEL:
        case VCOLMAX:
        case VTIMER:
        case VRTIMER:
        case VOICE:
        case VMAXDEG:
        case VMAXMULT:
        case TRACE:
        case VSHORTOUT:
        case VNOETHER:
        case VMINPOLY:
        case 0:
          //attribute=NULL; // will be done by Init() at the end
          break;
        default:
        {
          attribute->killAll(r);
        }
      }
    }
  }
  Subexpr h;
  while (e!=NULL)
  {
    h=e->next;
    omFreeBin((ADDRESS)e, sSubexpr_bin);
    e=h;
  }
  //rtyp=NONE; // will be done by Init() at the end
  if (next!=NULL)
  {
    leftv tmp_n;
    do
    {
      tmp_n=next->next;
      //next->name=NULL;
      next->next=NULL;
      next->CleanUp(r);
      omFreeBin((ADDRESS)next, sleftv_bin);
      next=tmp_n;
    } while (next!=NULL);
  }
  Init();
}

BOOLEAN sleftv::RingDependend()
{
  int rt=Typ();
  if(::RingDependend(rt))
    return TRUE;
  if (rt==LIST_CMD)
    return lRingDependend((lists)Data());
  if (this->next!=NULL)
    return this->next->RingDependend();
  return FALSE;
}

static inline void * s_internalCopy(const int t,  void *d)
{
  switch (t)
  {
    case CRING_CMD:
      {
        coeffs cf=(coeffs)d;
        cf->ref++;
        return (void*)d;
      }
#ifdef SINGULAR_4_2
    case CNUMBER_CMD:
      return (void*)n2Copy((number2)d);
    case CPOLY_CMD:
      return (void*)p2Copy((poly2)d);
    case CMATRIX_CMD: // like BIGINTMAT
#endif
    case BIGINTMAT_CMD:
      return (void*)bimCopy((bigintmat *)d);
    case BUCKET_CMD:
      return (void*)sBucketCopy((sBucket_pt)d);
    case INTVEC_CMD:
    case INTMAT_CMD:
      return (void *)ivCopy((intvec *)d);
    case MATRIX_CMD:
      return (void *)mp_Copy((matrix)d, currRing);
    case SMATRIX_CMD:
    case IDEAL_CMD:
    case MODUL_CMD:
      return  (void *)idCopy((ideal)d);
    case STRING_CMD:
        return (void *)omStrDup((char *)d);
    case PACKAGE_CMD:
      return  (void *)paCopy((package) d);
    case PROC_CMD:
      return  (void *)piCopy((procinfov) d);
    case POLY_CMD:
    case VECTOR_CMD:
      return  (void *)pCopy((poly)d);
    case INT_CMD:
      return  d;
    case NUMBER_CMD:
      return  (void *)nCopy((number)d);
    case BIGINT_CMD:
      return  (void *)n_Copy((number)d, coeffs_BIGINT);
    case MAP_CMD:
      return  (void *)maCopy((map)d, currRing);
    case LIST_CMD:
      return  (void *)lCopy((lists)d);
    case LINK_CMD:
      return (void *)slCopy((si_link) d);
    case RING_CMD:
      {
        ring r=(ring)d;
        if (r!=NULL)
        {
          r->ref++;
          //Print("s_internalCopy:+  ring %d, ref %d\n",r,r->ref);
        }
        return d;
      }
    case RESOLUTION_CMD:
      return (void*)syCopy((syStrategy)d);
    case DEF_CMD:
    case NONE:
    case 0: /*!
!
 type in error case 

*/
      break; /*!
!
 error recovery: do nothing 

*/
    //case COMMAND:
    default:
    {
      if (t>MAX_TOK)
      {
        blackbox *b=getBlackboxStuff(t);
        if (b!=NULL) return b->blackbox_Copy(b,d);
        return NULL;
      }
      else
      Warn("s_internalCopy: cannot copy type %s(%d)",
            Tok2Cmdname(t),t);
    }
  }
  return NULL;
}

void s_internalDelete(const int t,  void *d, const ring r)
{
  assume(d!=NULL);
  switch (t)
  {
    case CRING_CMD:
      {
        coeffs cf=(coeffs)d;
        if ((cf->ref<1)&&
        ((cf->type <=n_GF)
          ||((cf->type >=n_long_C)&&(cf->type <=n_CF))))
        {
          Warn("cannot kill `%s`",nCoeffName(cf));
        }
        else // allow nKillChar for n_long_R, extensions, and user defined:
          nKillChar((coeffs)d);
        break;
      }
#ifdef SINGULAR_4_2
    case CNUMBER_CMD:
      {
        number2 n=(number2)d;
        n2Delete(n);
        break;
      }
    case CPOLY_CMD:
      {
        poly2 n=(poly2)d;
        p2Delete(n);
        break;
      }
    case CMATRIX_CMD: //like BIGINTMAT
#endif
    case BIGINTMAT_CMD:
    {
      bigintmat *v=(bigintmat*)d;
      delete v;
      break;
    }
    case BUCKET_CMD:
    {
      sBucket_pt b=(sBucket_pt)d;
      sBucketDeleteAndDestroy(&b);
      break;
    }
    case INTVEC_CMD:
    case INTMAT_CMD:
    {
      intvec *v=(intvec*)d;
      delete v;
      break;
    }
    case MAP_CMD:
    {
      map m=(map)d;
      omFreeBinAddr((ADDRESS)m->preimage);
      m->preimage=NULL;
      /*!
!
 no break: continue as IDEAL

*/
    }
    case SMATRIX_CMD:
    case MATRIX_CMD:
    case IDEAL_CMD:
    case MODUL_CMD:
    {
      ideal i=(ideal)d;
      id_Delete(&i,r);
      break;
    }
    case STRING_CMD:
      omFree(d);
      break;
    //case PACKAGE_CMD:
    //  return  (void *)paCopy((package) d);
    case PROC_CMD:
      piKill((procinfo*)d);
      break;
    case POLY_CMD:
    case VECTOR_CMD:
    {
      poly p=(poly)d;
      p_Delete(&p,r);
      break;
    }
    case NUMBER_CMD:
    {
      number n=(number)d;
      n_Delete(&n,r->cf);
      break;
    }
    case BIGINT_CMD:
    {
      number n=(number)d;
      n_Delete(&n,coeffs_BIGINT);
      break;
    }
    case LIST_CMD:
    {
      lists l=(lists)d;
      l->Clean(r);
      break;
    }
    case LINK_CMD:
    {
      si_link l=(si_link)d;
      slKill(l);
      break;
    }
    case RING_CMD:
    {
      ring R=(ring)d;
      if ((R!=currRing)||(R->ref>=0))
        rKill(R);
      #ifdef TEST
      else
        Print("currRing? ref=%d\n",R->ref);
      #endif
      break;
    }
    case RESOLUTION_CMD:
    {
      syStrategy s=(syStrategy)d;
      if (s!=NULL) syKillComputation(s,r);
      break;
    }
    case COMMAND:
    {
     command cmd=(command)d;
     if (cmd->arg1.rtyp!=0) cmd->arg1.CleanUp(r);
     if (cmd->arg2.rtyp!=0) cmd->arg2.CleanUp(r);
     if (cmd->arg3.rtyp!=0) cmd->arg3.CleanUp(r);
     omFreeBin((ADDRESS)d, sip_command_bin);
     break;
    }
    case INT_CMD:
    case DEF_CMD:
    case ALIAS_CMD:
    case PACKAGE_CMD:
    case IDHDL:
    case NONE:
    case ANY_TYPE:
    case VECHO:
    case VPRINTLEVEL:
    case VCOLMAX:
    case VTIMER:
    case VRTIMER:
    case VOICE:
    case VMAXDEG:
    case VMAXMULT:
    case TRACE:
    case VSHORTOUT:
    case VNOETHER:
    case VMINPOLY:
    case 0: /*!
!
 type in error case 

*/
      break; /*!
!
 error recovery: do nothing 

*/
    //case COMMAND:
    //case COMMAND:
    default:
    {
      if (t>MAX_TOK)
      {
        blackbox *b=getBlackboxStuff(t);
        if (b!=NULL) b->blackbox_destroy(b,d);
        break;
      }
      else
      Warn("s_internalDelete: cannot delete type %s(%d)",
            Tok2Cmdname(t),t);
    }
  }
}

void * slInternalCopy(leftv source, const int t, void *d, Subexpr e)
{
  if (t==STRING_CMD)
  {
      if ((e==NULL)
      || (source->rtyp==LIST_CMD)
      || ((source->rtyp==IDHDL)
          &&((IDTYP((idhdl)source->data)==LIST_CMD)
            || (IDTYP((idhdl)source->data)>MAX_TOK)))
      || (source->rtyp>MAX_TOK))
        return (void *)omStrDup((char *)d);
      else if (e->next==NULL)
      {
        char *s=(char*)omAllocBin(size_two_bin);
        s[0]=*(char *)d;
        s[1]='\0';
        return s;
      }
      #ifdef TEST
      else
      {
        Werror("not impl. string-op in `%s`",my_yylinebuf);
        return NULL;
      }
      #endif
  }
  return s_internalCopy(t,d);
}

void sleftv::Copy(leftv source)
{
  Init();
  rtyp=source->Typ();
  void *d=source->Data();
  if(!errorreported)
  {
    if (rtyp==BUCKET_CMD)
    {
      rtyp=POLY_CMD;
      data=(void*)pCopy(sBucketPeek((sBucket_pt)d));
    }
    else
      data=s_internalCopy(rtyp,d);
    if ((source->attribute!=NULL)||(source->e!=NULL))
      attribute=source->CopyA();
    flag=source->flag;
    if (source->next!=NULL)
    {
      next=(leftv)omAllocBin(sleftv_bin);
      next->Copy(source->next);
    }
  }
}

void * sleftv::CopyD(int t)
{
  if ((rtyp!=IDHDL)&&(rtyp!=ALIAS_CMD)&&(e==NULL))
  {
    if (iiCheckRing(t)) return NULL;
    void *x = data;
    if (rtyp==VNOETHER) x = (void *)pCopy((currRing->ppNoether));
    else if ((rtyp==VMINPOLY) && nCoeff_is_algExt(currRing->cf) && (!nCoeff_is_GF(currRing->cf)))
    {
      const ring A = currRing->cf->extRing;

      assume( A != NULL );
      assume( A->qideal != NULL );

      x=(void *)p_Copy(A->qideal->m[0], A);
    }
    data=NULL;
    return x;
  }
  void *d=Data(); // will also do a iiCheckRing
  if ((!errorreported) && (d!=NULL)) return slInternalCopy(this,t,d,e);
  return NULL;
}

//void * sleftv::CopyD()
//{
  //if ((rtyp!=IDHDL)&&(e==NULL)
  //&&(rtyp!=VNOETHER)&&(rtyp!=VMINPOLY))
  //{
  //  void *x=data;
  //  data=NULL;
  //  return x;
  //}
//  return CopyD(Typ());
//}

attr sleftv::CopyA()
{
  attr *a=Attribute();
  if ((a!=NULL) && (*a!=NULL))
    return (*a)->Copy();
  return NULL;
}

char *  sleftv::String(void *d, BOOLEAN typed, int dim)
{
#ifdef SIQ
  if (rtyp==COMMAND)
  {
    ::Print("##command %d\n",((command)data)->op);
    if (((command)data)->arg1.rtyp!=0)
      ((command)data)->arg1.Print(NULL,2);
    if (((command)data)->arg2.rtyp!=0)
      ((command)data)->arg2.Print(NULL,2);
    if (((command)data)->arg3.rtyp==0)
      ((command)data)->arg3.Print(NULL,2);
    PrintS("##end\n");
    return omStrDup("");
  }
#endif
  if (d==NULL) d=Data();
  if (!errorreported)
  {
    char *s;
    int t=Typ();
    switch (t /*!
!
Typ()

*/)
    {
        case INT_CMD:
          if (typed)
          {
            s=(char *)omAlloc(MAX_INT_LEN+7);
            sprintf(s,"int(%d)",(int)(long)d);
          }
          else
          {
            s=(char *)omAlloc(MAX_INT_LEN+2);
            sprintf(s,"%d",(int)(long)d);
          }
          return s;

        case STRING_CMD:
          if (d == NULL)
          {
            if (typed) return omStrDup("\"\"");
            return omStrDup("");
          }
          if (typed)
          {
            s = (char*) omAlloc(strlen((char*) d) + 3);
            sprintf(s,"\"%s\"", (char*) d);
            return s;
          }
          else
          {
            return omStrDup((char*)d);
          }

        case POLY_CMD:
        case VECTOR_CMD:
          if (typed)
          {
            char* ps = pString((poly) d);
            s = (char*) omAlloc(strlen(ps) + 10);
            sprintf(s,"%s(%s)", (t /*!
!
Typ()

*/ == POLY_CMD ? "poly" : "vector"), ps);
            omFree(ps);
            return s;
          }
          else
            return pString((poly)d);

        case CRING_CMD:
          return nCoeffString((coeffs)d);
        #ifdef SINGULAR_4_2
        case CNUMBER_CMD:
          return n2String((number2)d,typed);
        case CMATRIX_CMD:
          {
            bigintmat *b=(bigintmat*)d;
            return b->String();
          }
        #endif

        case NUMBER_CMD:
          StringSetS((char*) (typed ? "number(" : ""));
          if((rtyp==VMINPOLY)&&(rField_is_GF(currRing)))
          {
            nfShowMipo(currRing->cf);
          }
          else
          {
            nWrite((number)d);
          }
          StringAppendS((char*) (typed ? ")" : ""));
          return StringEndS();

        case BIGINT_CMD:
          {
          StringSetS((char*) (typed ? "bigint(" : ""));
          number nl=(number)d;
          n_Write(nl,coeffs_BIGINT);
          StringAppendS((char*) (typed ? ")" : ""));
          return StringEndS();
          }
        case BUCKET_CMD:
          return sBucketString((sBucket_pt)d);
        case MATRIX_CMD:
          s= iiStringMatrix((matrix)d,dim, currRing);
          if (typed)
          {
            char* ns = (char*) omAlloc(strlen(s) + 40);
            sprintf(ns, "matrix(ideal(%s),%d,%d)", s,
                    ((ideal) d)->nrows, ((ideal) d)->ncols);
            omCheckAddr(ns);
            return ns;
          }
          else
          {
            return omStrDup(s);
          }

        case IDEAL_CMD:
        case MAP_CMD:
        case MODUL_CMD:
        case SMATRIX_CMD:
          s= iiStringMatrix((matrix)d,dim, currRing);
          if (typed)
          {
            char* ns = (char*) omAlloc(strlen(s) + 10);
            if ((t/*!
!
Typ()

*/==IDEAL_CMD)||(t==MAP_CMD))
              sprintf(ns, "ideal(%s)", s);
            else /*!
!
MODUL_CMD, SMATRIX_CMD 

*/
              sprintf(ns, "module(%s)", s);
            omFree(s);
            omCheckAddr(ns);
            return ns;
          }
          return s;

        case INTVEC_CMD:
        case INTMAT_CMD:
        {
          intvec *v=(intvec *)d;
          s = v->String(dim);
          if (typed)
          {
            char* ns;
            if (t/*!
!
Typ()

*/ == INTMAT_CMD)
            {
              ns = (char*) omAlloc(strlen(s) + 40);
              sprintf(ns, "intmat(intvec(%s),%d,%d)", s, v->rows(), v->cols());
            }
            else
            {
              ns = (char*) omAlloc(strlen(s) + 10);
              sprintf(ns, "intvec(%s)", s);
            }
            omCheckAddr(ns);
            omFree(s);
            return ns;
          }
          else
            return s;
        }
        case BIGINTMAT_CMD:
        {
          bigintmat *bim=(bigintmat*)d;
          s = bim->String();
          if (typed)
          {
            char* ns = (char*) omAlloc0(strlen(s) + 40);
            sprintf(ns, "bigintmat(bigintvec(%s),%d,%d)", s, bim->rows(), bim->cols());
            omCheckAddr(ns);
            return ns;
          }
          else
            return omStrDup(s);
        }

        case RING_CMD:
          s  = rString((ring)d);

          if (typed)
          {
            char* ns;
            ring r=(ring)d;
            if (r->qideal!=NULL)
            {
              char* id = iiStringMatrix((matrix) ((ring) d)->qideal, dim,
                              currRing);
              ns = (char*) omAlloc(strlen(s) + strlen(id) + 20);
              sprintf(ns, "\"%s\";%sideal(%s)", s,(dim == 2 ? "\n" : " "), id);
            }
            else
            {
              ns = (char*) omAlloc(strlen(s) + 4);
              sprintf(ns, "\"%s\"", s);
            }
            omFree(s);
            omCheckAddr(ns);
            return ns;
          }
          return s;
        case RESOLUTION_CMD:
        {
          lists l = syConvRes((syStrategy)d);
          s = lString(l, typed, dim);
          l->Clean();
          return s;
        }

        case PROC_CMD:
        {
          procinfo* pi = (procinfo*) d;
          if((pi->language == LANG_SINGULAR) && (pi->data.s.body!=NULL))
            s = (pi->data.s.body);
          else
            s = (char *)"";
          if (typed)
          {
            char* ns = (char*) omAlloc(strlen(s) + 4);
            sprintf(ns, "\"%s\"", s);
            omCheckAddr(ns);
            return ns;
          }
          return omStrDup(s);
        }

        case LINK_CMD:
          s = slString((si_link) d);
          if (typed)
          {
            char* ns = (char*) omAlloc(strlen(s) + 10);
            sprintf(ns, "link(\"%s\")", s);
            omFreeBinAddr(s);
            omCheckAddr(ns);
            return ns;
          }
          return s;

        case LIST_CMD:
          return lString((lists) d, typed, dim);

        default:
          if(t> MAX_TOK)
          {
            blackbox *bb=getBlackboxStuff(t);
            if (bb!=NULL) return bb->blackbox_String(bb,d);
          }
    } /*!
!
 end switch: (Typ()) 

*/
  }
  return omStrDup("");
}


int  sleftv::Typ()
{
  if (e==NULL)
  {
    switch (rtyp)
    {
      case IDHDL:
        return IDTYP((idhdl)data);
      case ALIAS_CMD:
         {
           idhdl h=(idhdl)data;
           return  ((idhdl)h->data.ustring)->typ;
         }
      case VECHO:
      case VPRINTLEVEL:
      case VCOLMAX:
      case VTIMER:
      case VRTIMER:
      case VOICE:
      case VMAXDEG:
      case VMAXMULT:
      case TRACE:
      case VSHORTOUT:
        return INT_CMD;
      case VMINPOLY:
        data=NULL;
        return NUMBER_CMD;
      case VNOETHER:
        data=NULL;
        return POLY_CMD;
      //case COMMAND:
      //  return COMMAND;
      default:
        return rtyp;
    }
  }
  int r=0;
  int t=rtyp;
  void *d=data;
  if (t==IDHDL) t=IDTYP((idhdl)d);
  else if (t==ALIAS_CMD)
  { idhdl h=(idhdl)IDDATA((idhdl)data); t=IDTYP(h);d=IDDATA(h); }
  switch (t)
  {
#ifdef SINGULAR_4_2
    case CMATRIX_CMD:
    {
      bigintmat *b=(bigintmat*)d;
      if ((currRing!=NULL)&&(currRing->cf==b->basecoeffs()))
        return NUMBER_CMD;
      else
        return CNUMBER_CMD;
    }
#endif
    case INTVEC_CMD:
    case INTMAT_CMD:
      r=INT_CMD;
      break;
    case BIGINTMAT_CMD:
      r=BIGINT_CMD;
      break;
    case IDEAL_CMD:
    case MATRIX_CMD:
    case MAP_CMD:
    case SMATRIX_CMD:
      r=POLY_CMD;
      break;
    case MODUL_CMD:
      r=VECTOR_CMD;
      break;
    case STRING_CMD:
      r=STRING_CMD;
      break;
    default:
    {
      blackbox *b=NULL;
      if (t>MAX_TOK)
      {
        b=getBlackboxStuff(t);
      }
      if ((t==LIST_CMD)||((b!=NULL)&&BB_LIKE_LIST(b)))
      {
        lists l;
        if (rtyp==IDHDL) l=IDLIST((idhdl)d);
        else             l=(lists)d;
        if ((0<e->start)&&(e->start<=l->nr+1))
        {
          Subexpr tmp=l->m[e->start-1].e;
          l->m[e->start-1].e=e->next;
          r=l->m[e->start-1].Typ();
          e->next=l->m[e->start-1].e;
          l->m[e->start-1].e=tmp;
        }
        else
        {
          //Warn("out of range: %d not in 1..%d",e->start,l->nr+1);
          r=DEF_CMD;
        }
      }
      else
        Werror("cannot index type %s(%d)",Tok2Cmdname(t),t);
      break;
    }
  }
  return r;
}

int  sleftv::LTyp()
{
  lists l=NULL;
  int r;
  if (rtyp==LIST_CMD)
    l=(lists)data;
  else if ((rtyp==IDHDL)&& (IDTYP((idhdl)data)==LIST_CMD))
    l=IDLIST((idhdl)data);
  else
    return Typ();
  //if (l!=NULL)
  {
    if ((e!=NULL) && (e->next!=NULL))
    {
      if ((0<e->start)&&(e->start<=l->nr+1))
      {
        l->m[e->start-1].e=e->next;
        r=l->m[e->start-1].LTyp();
        l->m[e->start-1].e=NULL;
      }
      else
      {
        //Warn("out of range: %d not in 1..%d",e->start,l->nr+1);
        r=NONE;
      }
      return r;
    }
    return LIST_CMD;
  }
  return Typ();
}

#ifdef SINGULAR_4_2
static snumber2 iiNumber2Data[4];
static int iiCmatrix_index=0;
#endif
void * sleftv::Data()
{
  if ((rtyp!=IDHDL) && iiCheckRing(rtyp))
     return NULL;
  if (e==NULL)
  {
    switch (rtyp)
    {
      case ALIAS_CMD:
      {
        idhdl h=(idhdl)data;
        return  ((idhdl)h->data.ustring)->data.ustring;
      }
      case VECHO:      return (void *)(long)si_echo;
      case VPRINTLEVEL:return (void *)(long)printlevel;
      case VCOLMAX:    return (void *)(long)colmax;
      case VTIMER:     return (void *)(long)getTimer();
      case VRTIMER:    return (void *)(long)getRTimer();
      case VOICE:      return (void *)(long)(myynest+1);
      case VMAXDEG:    return (void *)(long)Kstd1_deg;
      case VMAXMULT:   return (void *)(long)Kstd1_mu;
      case TRACE:      return (void *)(long)traceit;
      case VSHORTOUT:  return (void *)(long)(currRing != NULL ? currRing->ShortOut : 0);
      case VMINPOLY:
        if ( (currRing != NULL)  && nCoeff_is_algExt(currRing->cf) && !nCoeff_is_GF(currRing->cf))
        {
          /*!
!
 Q(a), Fp(a), but not GF(q) 

*/
          const ring A = currRing->cf->extRing;

          assume( A != NULL );
          assume( A->qideal != NULL );

          return (void *)A->qideal->m[0];
        }
        else
          return (void *)nInit(0);

      case VNOETHER:   return (void *) (currRing->ppNoether);
      case IDHDL:
        return IDDATA((idhdl)data);
      case COMMAND:
        //return NULL;
      default:
        return data;
    }
  }
  /*!
!
 e != NULL : 

*/
  int t=rtyp;
  void *d=data;
  if (t==IDHDL)
  {
    t=((idhdl)data)->typ;
    d=IDDATA((idhdl)data);
  }
  else if (t==ALIAS_CMD)
  {
    idhdl h=(idhdl)IDDATA((idhdl)data);
    t=IDTYP(h);
    d=IDDATA(h);
  }
  if (iiCheckRing(t))
    return NULL;
  char *r=NULL;
  int index=e->start;
  switch (t)
  {
    case INTVEC_CMD:
    {
      intvec *iv=(intvec *)d;
      if ((index<1)||(index>iv->length()))
      {
        if (!errorreported)
          Werror("wrong range[%d] in intvec %s(%d)",index,this->Name(),iv->length());
      }
      else
        r=(char *)(long)((*iv)[index-1]);
      break;
    }
    case INTMAT_CMD:
    {
      intvec *iv=(intvec *)d;
      if ((index<1)
         ||(index>iv->rows())
         ||(e->next->start<1)
         ||(e->next->start>iv->cols()))
      {
        if (!errorreported)
        Werror("wrong range[%d,%d] in intmat %s(%dx%d)",index,e->next->start,
                                           this->Name(),iv->rows(),iv->cols());
      }
      else
        r=(char *)(long)(IMATELEM((*iv),index,e->next->start));
      break;
    }
    case BIGINTMAT_CMD:
    {
      bigintmat *m=(bigintmat *)d;
      if ((index<1)
         ||(index>m->rows())
         ||(e->next->start<1)
         ||(e->next->start>m->cols()))
      {
        if (!errorreported)
        Werror("wrong range[%d,%d] in bigintmat %s(%dx%d)",index,e->next->start,
                                                     this->Name(),m->rows(),m->cols());
      }
      else
        r=(char *)(BIMATELEM((*m),index,e->next->start));
      break;
    }
#ifdef SINGULAR_4_2
    case CMATRIX_CMD:
    {
      bigintmat *m=(bigintmat *)d;
      if ((index<1)
         ||(index>m->rows())
         ||(e->next->start<1)
         ||(e->next->start>m->cols()))
      {
        if (!errorreported)
        Werror("wrong range[%d,%d] in matrix %s(%dx%d)",index,e->next->start,
                                                     this->Name(),m->rows(),m->cols());
      }
      else
      {
        iiNumber2Data[iiCmatrix_index].cf=m->basecoeffs();
        iiNumber2Data[iiCmatrix_index].n=BIMATELEM((*m),index,e->next->start);
        r=(char*)&iiNumber2Data[iiCmatrix_index];
        iiCmatrix_index=(iiCmatrix_index+1) % 4;
      }
      break;
    }
#endif
    case IDEAL_CMD:
    case MODUL_CMD:
    case MAP_CMD:
    {
      ideal I=(ideal)d;
      if ((index<1)||(index>IDELEMS(I)))
      {
        if (!errorreported)
          Werror("wrong range[%d] in ideal/module %s(%d)",index,this->Name(),IDELEMS(I));
      }
      else
        r=(char *)I->m[index-1];
      break;
    }
    case SMATRIX_CMD:
    {
      ideal I=(ideal)d;
      int c;
      sleftv tmp;
      tmp.Init();
      tmp.rtyp=POLY_CMD;
      if ((index>0)&& (index<=I->rank)
      && (e->next!=NULL)
      && ((c=e->next->start)>0) &&(c<=IDELEMS(I)))
      {
        r=(char*)SMATELEM(I,index-1,c-1,currRing);
      }
      else
      {
        r=NULL;
      }
      tmp.data=r;
      if ((rtyp==IDHDL)||(rtyp==SMATRIX_CMD))
      {
        tmp.next=next; next=NULL;
        d=NULL;
        CleanUp();
        memcpy(this,&tmp,sizeof(tmp));
      }
      // and, remember, r is also the result...
      else
      {
        // ???
        // here we still have a memory leak...
        // example: list L="123","456";
        // L[1][2];
        // therefore, it should never happen:
        assume(0);
        // but if it happens: here is the temporary fix:
        // omMarkAsStaticAddr(r);
      }
      break;
    }
    case STRING_CMD:
    {
      // this was a memory leak
      // we evalute it, cleanup and replace this leftv by it's evalutated form
      // the evalutated form will be build in tmp
      sleftv tmp;
      tmp.Init();
      tmp.rtyp=STRING_CMD;
      r=(char *)omAllocBin(size_two_bin);
      if ((index>0)&& (index<=(int)strlen((char *)d)))
      {
        r[0]=*(((char *)d)+index-1);
        r[1]='\0';
      }
      else
      {
        r[0]='\0';
      }
      tmp.data=r;
      if ((rtyp==IDHDL)||(rtyp==STRING_CMD))
      {
        tmp.next=next; next=NULL;
        //if (rtyp==STRING_CMD) { omFree((ADDRESS)data); }
        //data=NULL;
        d=NULL;
        CleanUp();
        memcpy(this,&tmp,sizeof(tmp));
      }
      // and, remember, r is also the result...
      else
      {
        // ???
        // here we still have a memory leak...
        // example: list L="123","456";
        // L[1][2];
        // therefore, it should never happen:
        assume(0);
        // but if it happens: here is the temporary fix:
        // omMarkAsStaticAddr(r);
      }
      break;
    }
    case MATRIX_CMD:
    {
      if ((index<1)
         ||(index>MATROWS((matrix)d))
         ||(e->next->start<1)
         ||(e->next->start>MATCOLS((matrix)d)))
      {
        if (!errorreported)
          Werror("wrong range[%d,%d] in matrix %s(%dx%d)",
                  index,e->next->start,
                  this->Name(),
                  MATROWS((matrix)d),MATCOLS((matrix)d));
      }
      else
        r=(char *)MATELEM((matrix)d,index,e->next->start);
      break;
    }
    default:
    {
      blackbox *b=NULL;
      if (t>MAX_TOK)
      {
        b=getBlackboxStuff(t);
      }
      if ((t==LIST_CMD)||((b!=NULL)&&(BB_LIKE_LIST(b))))
      {
        lists l=(lists)d;
        if ((0<index)&&(index<=l->nr+1))
        {
          if ((e->next!=NULL)
          && (l->m[index-1].rtyp==STRING_CMD))
          // string[..].Data() modifies sleftv, so let's do it ourself
          {
            char *dd=(char *)l->m[index-1].data;
            int j=e->next->start-1;
            r=(char *)omAllocBin(size_two_bin);
            if ((j>=0) && (j<(int)strlen(dd)))
            {
              r[0]=*(dd+j);
              r[1]='\0';
            }
            else
            {
              r[0]='\0';
            }
          }
          else
          {
            Subexpr tmp=l->m[index-1].e;
            l->m[index-1].e=e->next;
            r=(char *)l->m[index-1].Data();
            e->next=l->m[index-1].e;
            l->m[index-1].e=tmp;
          }
        }
        else //if (!errorreported)
          Werror("wrong range[%d] in list %s(%d)",index,this->Name(),l->nr+1);
      }
      else
        Werror("cannot index %s of type %s(%d)",this->Name(),Tok2Cmdname(t),t);
      break;
    }
  }
  return r;
}

attr * sleftv::Attribute()
{
  if (e==NULL) return &attribute;
  if ((rtyp==LIST_CMD)
  ||((rtyp==IDHDL)&&(IDTYP((idhdl)data)==LIST_CMD))
  || (rtyp>MAX_TOK)
  || ((rtyp==IDHDL)&&(IDTYP((idhdl)data)>MAX_TOK)))
  {
    leftv v=LData();
    return &(v->attribute);
  }
  return NULL;
}

leftv sleftv::LData()
{
  if (e!=NULL)
  {
    lists l=NULL;
    blackbox *b=getBlackboxStuff(rtyp);

    if ((rtyp==LIST_CMD)
    || ((b!=NULL)&&(BB_LIKE_LIST(b))))
      l=(lists)data;
    else if ((rtyp==IDHDL)&& (IDTYP((idhdl)data)==LIST_CMD))
      l=IDLIST((idhdl)data);
    else if ((rtyp==IDHDL)&& (IDTYP((idhdl)data)>MAX_TOK))
    {
      b=getBlackboxStuff(IDTYP((idhdl)data));
      if (BB_LIKE_LIST(b)) l=IDLIST((idhdl)data);
    }
    else if (rtyp==ALIAS_CMD)
    {
      idhdl h=(idhdl)data;
      l= (lists)(((idhdl)h->data.ustring)->data.ustring);
    }
    if (l!=NULL)
    {
      if ((0>=e->start)||(e->start>l->nr+1))
        return NULL;
      if (e->next!=NULL)
      {
        l->m[e->start-1].e=e->next;
        leftv r=l->m[e->start-1].LData();
        l->m[e->start-1].e=NULL;
        return r;
      }
      return &(l->m[e->start-1]);
    }
  }
  return this;
}

#if 0
leftv sleftv::LHdl()
{
  if (e!=NULL)
  {
    lists l=NULL;

    if (rtyp==LIST_CMD)
      l=(lists)data;
    if ((rtyp==IDHDL)&& (IDTYP((idhdl)data)==LIST_CMD))
      l=IDLIST((idhdl)data);
    if (l!=NULL)
    {
      if ((0>=e->start)||(e->start>l->nr+1))
        return NULL;
      if (e->next!=NULL)
      {
        l->m[e->start-1].e=e->next;
        leftv r=l->m[e->start-1].LHdl();
        l->m[e->start-1].e=NULL;
        return r;
      }
      return &(l->m[e->start-1]);
    }
  }
  return this;
}
#endif

BOOLEAN assumeStdFlag(leftv h)
{
  if (h->e!=NULL)
  {
    leftv hh=h->LData();
    if (h!=hh) return assumeStdFlag(h->LData());
  }
  if (!hasFlag(h,FLAG_STD))
  {
    if (!TEST_VERB_NSB)
    {
      if (TEST_V_ALLWARN)
        Warn("%s is no standard basis in >>%s<<",h->Name(),my_yylinebuf);
      else
        Warn("%s is no standard basis",h->Name());
    }
    return FALSE;
  }
  return TRUE;
}

/*!
!
2
* transforms a name (as an string created by omAlloc or omStrDup)
* into an expression (sleftv), deletes the string
* utility for grammar and iparith


*/
void syMake(leftv v,const char * id, package pa)
{
  /*!
!
 resolv an identifier: (to DEF_CMD, if siq>0)
  * 1) reserved id: done by scanner
  * 2) `basering` / 'Current`
  * 3) existing identifier, local
  * 4) ringvar, ringpar, local ring
  * 5) existing identifier, global
  * 6) monom (resp. number), local ring: consisting of:
  * 6') ringvar,  ringpar,global ring
  * 6'') monom (resp. number), local ring
  * 7) monom (resp. number), non-local ring
  * 8) basering
  * 9) `_`
  * 10) everything else is of type 0
  

*/
#ifdef TEST
  if ((*id<' ')||(*id>(char)126))
  {
    Print("wrong id :%s:\n",id);
  }
#endif
  idhdl save_ring=currRingHdl;
  v->Init();
  if(pa != NULL)
  {
    v->req_packhdl = pa;
  }
  else v->req_packhdl = currPack;
//  if (v->req_packhdl!=basePack)
//    Print("search %s in %s\n",id,v->req_packhdl->libname);
  idhdl h=NULL;
#ifdef SIQ
  if (siq<=0)
#endif
  {
    if (!isdigit(id[0]))
    {
      if (strcmp(id,"basering")==0)
      {
        if (currRingHdl!=NULL)
        {
          if (id!=IDID(currRingHdl)) omFreeBinAddr((ADDRESS)id);
          h=currRingHdl;
          goto id_found;
        }
        else
        {
          v->name = id;
          return; /*!
!
 undefined 

*/
        }
      }
      else if (strcmp(id,"Current")==0)
      {
        if (currPackHdl!=NULL)
        {
          omFreeBinAddr((ADDRESS)id);
          h=currPackHdl;
          goto id_found;
        }
        else
        {
          v->name = id;
          return; /*!
!
 undefined 

*/
        }
      }
      if(v->req_packhdl!=currPack)
      {
        h=v->req_packhdl->idroot->get(id,myynest);
      }
      else
      h=ggetid(id);
      /*!
!
 3) existing identifier, local 

*/
      if ((h!=NULL) && (IDLEV(h)==myynest))
      {
        if (id!=IDID(h)) omFreeBinAddr((ADDRESS)id); /*!
!
assume strlen(id) <1000 

*/
        goto id_found;
      }
    }
    if (yyInRingConstruction)
    {
      currRingHdl=NULL;
    }
    /*!
!
 4. local ring: ringvar 

*/
    if ((currRingHdl!=NULL) && (IDLEV(currRingHdl)==myynest)
    /*!
!
&& (!yyInRingConstruction)

*/)
    {
      int vnr;
      if ((vnr=r_IsRingVar(id, currRing->names,currRing->N))>=0)
      {
        poly p=pOne();
        pSetExp(p,vnr+1,1);
        pSetm(p);
        v->data = (void *)p;
        v->name = id;
        v->rtyp = POLY_CMD;
        return;
      }
      if((n_NumberOfParameters(currRing->cf)>0)
      &&((vnr=r_IsRingVar(id, (char**)n_ParameterNames(currRing->cf),
                              n_NumberOfParameters(currRing->cf))>=0)))
      {
        BOOLEAN ok=FALSE;
        poly p = pmInit(id,ok);
        if (ok && (p!=NULL))
        {
          v->data = pGetCoeff(p);
          pGetCoeff(p)=NULL;
          pLmFree(p);
          v->rtyp = NUMBER_CMD;
          v->name = id;
          return;
        }
      }
    }
    /*!
!
 5. existing identifier, global 

*/
    if (h!=NULL)
    {
      if (id!=IDID(h)) omFreeBinAddr((ADDRESS)id);  /*!
!
assume strlen(id) <1000 

*/
      goto id_found;
    }
    /*!
!
 6. local ring: number/poly 

*/
    if ((currRingHdl!=NULL) && (IDLEV(currRingHdl)==myynest)
    #ifdef HAVE_SHIFTBBA
    && (currRing->isLPring==0)
    #endif
    )
    {
      BOOLEAN ok=FALSE;
      /*!
!
poly p = (!yyInRingConstruction) ? pmInit(id,ok) : (poly)NULL;

*/
      poly p = pmInit(id,ok);
      if (ok)
      {
        if (p==NULL)
        {
          v->data = (void *)nInit(0);
          v->rtyp = NUMBER_CMD;
          #ifdef HAVE_PLURAL
          // in this case we may have monomials equal to 0 in p_Read
          v->name = id;
          #else
          omFreeBinAddr((ADDRESS)id);
          #endif
        }
        else if (pIsConstant(p))
        {
          v->data = pGetCoeff(p);
          pGetCoeff(p)=NULL;
          pLmFree(p);
          v->rtyp = NUMBER_CMD;
          v->name = id;
        }
        else
        {
          v->data = p;
          v->rtyp = POLY_CMD;
          v->name = id;
        }
        return;
      }
    }
    /*!
!
 7. non-local ring: number/poly 

*/
    {
      BOOLEAN ok=FALSE;
      poly p = ((currRing!=NULL)     /*!
!
 ring required 

*/
               && (currRingHdl!=NULL)
               /*!
!
&& (!yyInRingConstruction) - not in decl 

*/
               && (IDLEV(currRingHdl)!=myynest)) /*!
!
 already in case 4/6 

*/
                     ? pmInit(id,ok) : (poly)NULL;
      if (ok)
      {
        if (p==NULL)
        {
          v->data = (void *)nInit(0);
          v->rtyp = NUMBER_CMD;
          omFreeBinAddr((ADDRESS)id);
        }
        else
        if (pIsConstant(p))
        {
          v->data = pGetCoeff(p);
          pGetCoeff(p)=NULL;
          pLmFree(p);
          v->rtyp = NUMBER_CMD;
          v->name = id;
        }
        else
        {
          v->data = p;
          v->rtyp = POLY_CMD;
          v->name = id;
        }
        //if (TEST_V_ALLWARN /*!
!
&& (myynest>0)

*/
        //&& ((r_IsRingVar(id, currRing->names,currRing->N)>=0)
        //  || ((n_NumberOfParameters(currRing->cf)>0)
        //     &&(r_IsRingVar(id, (char**)n_ParameterNames(currRing->cf),
        //                        n_NumberOfParameters(currRing->cf))>=0))))
        //{
        //// WARNING: do not use ring variable names in procedures
        //  Warn("use of variable >>%s<< in a procedure in line %s",id,my_yylinebuf);
        //}
        return;
      }
    }
    /*!
!
 8. basering ? 

*/
    if ((myynest>1)&&(currRingHdl!=NULL))
    {
      if (strcmp(id,IDID(currRingHdl))==0)
      {
        if (IDID(currRingHdl)!=id) omFreeBinAddr((ADDRESS)id); /*!
!
assume strlen (id) <1000 

*/
        h=currRingHdl;
        goto id_found;
      }
    }
    if((v->req_packhdl!=basePack) && (v->req_packhdl==currPack))
    {
      h=basePack->idroot->get(id,myynest);
      if (h!=NULL)
      {
        if (id!=IDID(h)) omFreeBinAddr((ADDRESS)id); /*!
!
assume strlen(id) <1000 

*/
        v->req_packhdl=basePack;
        goto id_found;
      }
    }
  }
#ifdef SIQ
  else
    v->rtyp=DEF_CMD;
#endif
  /*!
!
 9: _ 

*/
  if (strcmp(id,"_")==0)
  {
    omFreeBinAddr((ADDRESS)id);
    v->Copy(&sLastPrinted);
  }
  else
  {
    /*!
!
 10: everything else 

*/
    /*!
!
 v->rtyp = UNKNOWN;

*/
    v->name = id;
  }
  currRingHdl=save_ring;
  return;
id_found: // we have an id (in h) found, to set the data in from h
  if (IDTYP(h)!=ALIAS_CMD)
  {
    v->rtyp = IDHDL;
    v->flag = IDFLAG(h);
    v->attribute=IDATTR(h);
  }
  else
  {
    v->rtyp = ALIAS_CMD;
  }
  v->name = IDID(h);
  v->data = (char *)h;
  currRingHdl=save_ring;
}

int sleftv::Eval()
{
  BOOLEAN nok=FALSE;
  leftv nn=next;
  next=NULL;
  if(rtyp==IDHDL)
  {
    int t=Typ();
    if (t!=PROC_CMD)
    {
      void *d=CopyD(t);
      data=d;
      rtyp=t;
      name=NULL;
      e=NULL;
    }
  }
  else if (rtyp==COMMAND)
  {
    command d=(command)data;
    if(d->op==PROC_CMD) //assume d->argc==2
    {
      char *what=(char *)(d->arg1.Data());
      idhdl h=ggetid(what);
      if((h!=NULL)&&(IDTYP(h)==PROC_CMD))
      {
        nok=d->arg2.Eval();
        if(!nok)
        {
          nok=iiMake_proc(h,req_packhdl,&d->arg2);
          this->CleanUp(currRing);
          if (!nok)
          {
            memcpy(this,&iiRETURNEXPR,sizeof(sleftv));
            memset(&iiRETURNEXPR,0,sizeof(sleftv));
          }
        }
      }
      else nok=TRUE;
    }
    else if (d->op=='=') //assume d->argc==2
    {
      if ((d->arg1.rtyp!=IDHDL)&&(d->arg1.rtyp!=DEF_CMD))
      {
        nok=d->arg1.Eval();
      }
      if (!nok)
      {
        const char *n=d->arg1.name;
        nok=(n == NULL) || d->arg2.Eval();
        if (!nok)
        {
          int save_typ=d->arg1.rtyp;
          omCheckAddr((ADDRESS)n);
          if (d->arg1.rtyp!=IDHDL)
          syMake(&d->arg1,n);
          omCheckAddr((ADDRESS)d->arg1.name);
          if (d->arg1.rtyp==IDHDL)
          {
            n=omStrDup(IDID((idhdl)d->arg1.data));
            killhdl((idhdl)d->arg1.data);
            d->arg1.Init();
            //d->arg1.data=NULL;
            d->arg1.name=n;
          }
          d->arg1.rtyp=DEF_CMD;
          sleftv t;
          if(save_typ!=PROC_CMD) save_typ=d->arg2.rtyp;
          if (::RingDependend(d->arg2.rtyp))
            nok=iiDeclCommand(&t,&d->arg1,0,save_typ,&currRing->idroot);
          else
            nok=iiDeclCommand(&t,&d->arg1,0,save_typ,&IDROOT);
          memcpy(&d->arg1,&t,sizeof(sleftv));
          omCheckAddr((ADDRESS)d->arg1.name);
          nok=nok||iiAssign(&d->arg1,&d->arg2);
          omCheckIf(d->arg1.name != NULL,  // OB: ????
                    omCheckAddr((ADDRESS)d->arg1.name));
          if (!nok)
          {
            memset(&d->arg1,0,sizeof(sleftv));
            this->CleanUp();
            rtyp=NONE;
          }
        }
      }
      else nok=TRUE;
    }
    else
    {
      sleftv tmp; tmp.Init();
      int toktype=iiTokType(d->op);
      if ((toktype==CMD_M)
      ||( toktype==ROOT_DECL_LIST)
      ||( toktype==RING_DECL_LIST))
      {
        if (d->argc <=3)
        {
          if (d->argc>=1) nok=d->arg1.Eval();
          if ((!nok) && (d->argc>=2))
          {
            nok=d->arg2.Eval();
            d->arg1.next=(leftv)omAllocBin(sleftv_bin);
            memcpy(d->arg1.next,&d->arg2,sizeof(sleftv));
            d->arg2.Init();
          }
          if ((!nok) && (d->argc==3))
          {
            nok=d->arg3.Eval();
            d->arg1.next->next=(leftv)omAllocBin(sleftv_bin);
            memcpy(d->arg1.next->next,&d->arg3,sizeof(sleftv));
            d->arg3.Init();
          }
          if (d->argc==0)
            nok=nok||iiExprArithM(&tmp,NULL,d->op);
          else
            nok=nok||iiExprArithM(&tmp,&d->arg1,d->op);
        }
        else
        {
          nok=d->arg1.Eval();
          nok=nok||iiExprArithM(&tmp,&d->arg1,d->op);
        }
      }
      else if (d->argc==1)
      {
        nok=d->arg1.Eval();
        nok=nok||iiExprArith1(&tmp,&d->arg1,d->op);
      }
      else if(d->argc==2)
      {
        nok=d->arg1.Eval();
        nok=nok||d->arg2.Eval();
        nok=nok||iiExprArith2(&tmp,&d->arg1,d->op,&d->arg2);
      }
      else if(d->argc==3)
      {
        nok=d->arg1.Eval();
        nok=nok||d->arg2.Eval();
        nok=nok||d->arg3.Eval();
        nok=nok||iiExprArith3(&tmp,d->op,&d->arg1,&d->arg2,&d->arg3);
      }
      else if(d->argc!=0)
      {
        nok=d->arg1.Eval();
        nok=nok||iiExprArithM(&tmp,&d->arg1,d->op);
      }
      else // d->argc == 0
      {
        nok = iiExprArithM(&tmp, NULL, d->op);
      }
      this->CleanUp();
      memcpy(this,&tmp,sizeof(tmp));
    }
  }
  else if (((rtyp==0)||(rtyp==DEF_CMD))
    &&(name!=NULL))
  {
     syMake(this,name);
  }
#ifdef MDEBUG
  switch(Typ())
  {
    case NUMBER_CMD:
#ifdef LDEBUG
      nTest((number)Data());
#endif
      break;
    case BIGINT_CMD:
#ifdef LDEBUG
      n_Test((number)Data(),coeffs_BIGINT);
#endif
      break;
    case POLY_CMD:
      pTest((poly)Data());
      break;
    case IDEAL_CMD:
    case MODUL_CMD:
    case MATRIX_CMD:
      {
        ideal id=(ideal)Data();
        omCheckAddrSize(id,sizeof(*id));
        int i=id->ncols*id->nrows-1;
        for(;i>=0;i--) pTest(id->m[i]);
      }
      break;
  }
#endif
  if (nn!=NULL) nok=nok||nn->Eval();
  next=nn;
  return nok;
}

void * sattr::CopyA()
{
  omCheckAddrSize(this,sizeof(sattr));
  return s_internalCopy(atyp,data);
}

