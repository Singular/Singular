/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: handling of leftv
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifndef macintosh
#include <unistd.h>
#endif

#include "mod2.h"
#include "tok.h"
#include "ipid.h"
#include "intvec.h"
#include "mmemory.h"
#include "febase.h"
#include "polys.h"
#include "ideals.h"
#include "maps.h"
#include "matpol.h"
#include "kstd1.h"
#include "timer.h"
#include "ring.h"
#include "ffields.h"
#include "numbers.h"
#include "ipshell.h"
#include "lists.h"
#include "attrib.h"
#include "silink.h"
#include "syz.h"
#include "subexpr.h"

sleftv     sLastPrinted;
const char sNoName[]="_";
#ifdef SIQ
BOOLEAN siq=FALSE;
#endif

void sleftv::Set(int val)
{
  Init();
  rtyp = INT_CMD;
  data = (void *)val;
}

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
    char *s=ch;
    if (c->op>127) s=Tok2Cmdname(c->op);
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
    if ((t/*Typ()*/!=PROC_CMD)
    && (t/*Typ()*/!=BINARY_CMD))
    {
      const char *n=Name();
      char *s;
      void *d=Data();
      if (errorreported)
        return;
      if ((store!=NULL)&&(store!=this))
        store->CleanUp();

      switch (t /*=Typ()*/)
      {
        case UNKNOWN:
        case DEF_CMD:
        case PACKAGE_CMD:
        #ifdef HAVE_DLD
        case BINARY_CMD:
        #endif
          ::Print("%-*.*s`%s`",spaces,spaces," ",n);
          break;
        case NONE:
          return;
        case INTVEC_CMD:
        case INTMAT_CMD:
          ((intvec *)d)->show(t,spaces);
          break;
        case RING_CMD:
        case QRING_CMD:
          ::Print("%-*.*s",spaces,spaces," ");
          rWrite((ring)d);
          break;
        case MATRIX_CMD:
          iiWriteMatrix((matrix)d,n,2,spaces);
          break;
        case MAP_CMD:
        case MODUL_CMD:
        case IDEAL_CMD:
          iiWriteMatrix((matrix)d,n,1,spaces);
          break;
        case POLY_CMD:
        case VECTOR_CMD:
          ::Print("%-*.*s",spaces,spaces," ");
          pWrite0((poly)d);
          break;
        case RESOLUTION_CMD:
          syPrint((syStrategy)d);
          break;  
        case STRING_CMD:
          ::Print("%-*.*s%s",spaces,spaces," ",(char *)d);
          break;
       case INT_CMD:
          ::Print("%-*.*s%d",spaces,spaces," ",(int)d);
          break;
       case LINK_CMD:
          {
            si_link l=(si_link)d;
            ::Print("%-*.*s// type : %s\n",spaces,spaces," ",
                    slStatus(l, "type"));
            ::Print("%-*.*s// mode : %s\n",spaces,spaces," ",
                    slStatus(l, "mode"));
            ::Print("%-*.*s// name : %s\n",spaces,spaces," ",
                    slStatus(l, "name"));
            ::Print("%-*.*s// open : %s\n",spaces,spaces," ",
                    slStatus(l, "open"));
            ::Print("%-*.*s// read : %s\n",spaces,spaces," ",
                    slStatus(l, "read"));
            ::Print("%-*.*s// write: %s",spaces,spaces," ",
                    slStatus(l, "write"));
          break;
          }
        case NUMBER_CMD:
          s=String(d);
          if (s==NULL) return;
          ::Print("%-*.*s",spaces,spaces," ");
          PrintS(s);
          FreeL((ADDRESS)s);
          break;
        case LIST_CMD:
        {
          lists l=(lists)d;
          if (l->nr<0)
             ::Print("%-*.*sempty list\n",spaces,spaces," ");
          else
          {
            int i=0;
            for (;i<=l->nr;i++)
            {
              if (l->m[i].rtyp!=DEF_CMD)
              {
                ::Print("%-*.*s[%d]:\n",spaces,spaces," ",i+1);
                l->m[i].Print(NULL,spaces+3);
              }
            }
          }
          break;
        }
#ifdef TEST
        default:
          ::Print("Print:unknown type %s(%d)", Tok2Cmdname(t),t);
#endif
      } /* end switch: (Typ()) */
    }
  }
  if (next!=NULL)
  {
    if (t==COMMAND) PrintLn();
    else if (t!=LIST_CMD) PrintS(" ");
    next->Print(NULL,spaces);
  }
  else if (t!=LIST_CMD)
  {
    PrintLn();
  }  
#ifdef SIQ
  if (rtyp!=COMMAND)
#endif
  {
    if ((store!=NULL)
    && (store!=this)
    && (t/*Typ()*/!=LINK_CMD)
    && (t/*Typ()*/!=RING_CMD)
    && (t/*Typ()*/!=QRING_CMD)
    && (t/*Typ()*/!=PACKAGE_CMD)
    && (t/*Typ()*/!=PROC_CMD)
    && (t/*Typ()*/!=DEF_CMD)
    )
    {
      store->rtyp=t/*Typ()*/;
      store->data=CopyD();
    }
  }
}

void sleftv::CleanUp()
{
  if ((name!=NULL) && (name!=sNoName) && (rtyp!=IDHDL))
  {
    //::Print("free %x (%s)\n",name,name);
    FreeL((ADDRESS)name);
  }
  name=NULL;
  if (data!=NULL)
  {
    switch (rtyp)
    {
      case INTVEC_CMD:
      case INTMAT_CMD:
        delete (intvec *)data;
        break;
      case MAP_CMD:
        FreeL((ADDRESS)((map)data)->preimage);
        ((map)data)->preimage=NULL;
        // no break: kill the image as an ideal
      case MATRIX_CMD:
      case MODUL_CMD:
      case IDEAL_CMD:
        idDelete((ideal *)(&data));
        break;
      case STRING_CMD:
        FreeL((ADDRESS)data);
        break;
      case POLY_CMD:
      case VECTOR_CMD:
        pDelete((poly *)(&data));
        break;
      case NUMBER_CMD:
        nDelete((number *)(&data));
        break;
      case LIST_CMD:
        ((lists)data)->Clean();
        break;
      case QRING_CMD:
      case RING_CMD:
        rKill((ring)data);
        break;
      case LINK_CMD:
        slKill((si_link)data);
        break;
      case COMMAND:
      {
        command cmd=(command)data;
        if (cmd->arg1.rtyp!=0) cmd->arg1.CleanUp();
        if (cmd->arg2.rtyp!=0) cmd->arg2.CleanUp();
        if (cmd->arg3.rtyp!=0) cmd->arg3.CleanUp();
        Free((ADDRESS)data,sizeof(ip_command));
        break;
      }
      case RESOLUTION_CMD:
      {
        syKillComputation((syStrategy)data);
        break;
      }
#ifdef TEST
      // the following types do not take memory
      // or are not copied
      case IDHDL:
      case PACKAGE_CMD:
      case ANY_TYPE:
#ifdef SRING
      case VALTVARS:
#endif
      case VECHO:
      case VPAGELENGTH:
      case VPRINTLEVEL:
      case VCOLMAX:
      case VTIMER:
#ifdef HAVE_RTIMER
        case VRTIMER:
#endif          
      case VOICE:
      case VMAXDEG:
      case VMAXMULT:
      case TRACE:
      case VSHORTOUT:
      case VNOETHER:
      case VMINPOLY:
      case LIB_CMD:
      case 0:
      case INT_CMD:
        break;
      default:
#ifdef SIC
        if (rtyp<SIC_MASK)
#endif
        ::Print("CleanUp: unknown type %d\n",rtyp);  /* DEBUG */
#endif
    } /* end switch: (rtyp) */
    data=NULL;
  }
  if (attribute!=NULL)
  {
    switch (rtyp)
    {
      case PACKAGE_CMD:
      case IDHDL:
      case ANY_TYPE:
#ifdef SRING
      case VALTVARS:
#endif
      case VECHO:
      case VPAGELENGTH:
      case VPRINTLEVEL:
      case VCOLMAX:
      case VTIMER:
#ifdef HAVE_RTIMER
      case VRTIMER:
#endif          
      case VOICE:
      case VMAXDEG:
      case VMAXMULT:
      case TRACE:
      case VSHORTOUT:
      case VNOETHER:
      case VMINPOLY:
      case LIB_CMD:
      case 0:
        attribute=NULL;
        break;
      default:
      {
        attr t;
        while (attribute!=NULL)
        {
          t=attribute->next;
          attribute->kill();
          attribute=t;
        }
      }
    }
  }
  Subexpr h;
  while (e!=NULL)
  {
    h=e->next;
    Free((ADDRESS)e,sizeof(*e));
    e=h;
  }
  rtyp=NONE;
  if (next!=NULL)
  {
    next->name=NULL;
    next->CleanUp();
    Free((ADDRESS)next,sizeof(sleftv));
    next=NULL;
  }
}

void * slInternalCopy(leftv source, int t, void *d, Subexpr e)
{
  switch (t)
  {
    case INTVEC_CMD:
    case INTMAT_CMD:
      return (void *)ivCopy((intvec *)d);
    case MATRIX_CMD:
      return (void *)mpCopy((matrix)d);
    case IDEAL_CMD:
    case MODUL_CMD:
      return  (void *)idCopy((ideal)d);
    case STRING_CMD:
      if ((e==NULL)
      || (source->rtyp==LIST_CMD)
      || ((source->rtyp==IDHDL)&&(IDTYP((idhdl)source->data)==LIST_CMD)))
        return (void *)mstrdup((char *)d);
      else if (e->next==NULL)
      {
        char *s=(char *)AllocL(2);
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
    case PROC_CMD:
      return  (void *)mstrdup((char *)d);
    case POLY_CMD:
    case VECTOR_CMD:
      return  (void *)pCopy((poly)d);
    case INT_CMD:
      return  d;
    case NUMBER_CMD:
      return  (void *)nCopy((number)d);
    case MAP_CMD:
      return  (void *)maCopy((map)d);
    case LIST_CMD:
      return  (void *)lCopy((lists)d);
    case LINK_CMD:
      return (void *)slCopy((si_link) d);
    case RING_CMD:
    case QRING_CMD:
      {
        ring r=(ring)d;
        r->ref++;
        return d;
      }
    case RESOLUTION_CMD:
      return (void*)syCopy((syStrategy)d);
#ifdef TEST
    case DEF_CMD:
    case NONE:
      break; /* error recovery: do nothing */
    //case COMMAND:
    default:
      Warn("InternalCopy: cannot copy type %s(%d)",
            Tok2Cmdname(source->rtyp),source->rtyp);
#endif
  }
  return NULL;
}

void sleftv::Copy(leftv source)
{
  memset(this,0,sizeof(*this));
#ifdef SIC
  if (source->rtyp>SIC_MASK)
  {
    rtyp=source->rtyp;
    return;
  }
  else
#endif
  rtyp=source->Typ();
  void *d=source->Data();
  if(!errorreported)
  switch (rtyp)
  {
    case INTVEC_CMD:
    case INTMAT_CMD:
      data=(void *)ivCopy((intvec *)d);
      break;
    case MATRIX_CMD:
      data=(void *)mpCopy((matrix)d);
      break;
    case IDEAL_CMD:
    case MODUL_CMD:
      data= (void *)idCopy((ideal)d);
      break;
    case STRING_CMD:
      data= (void *)mstrdup((char *)d);
      break;
    case PROC_CMD:
      data= (void *)mstrdup((char *)d);
      break;
    case POLY_CMD:
    case VECTOR_CMD:
      data= (void *)pCopy((poly)d);
      break;
    case INT_CMD:
      data= d;
      break;
    case NUMBER_CMD:
      data= (void *)nCopy((number)d);
      break;
    case MAP_CMD:
      data= (void *)maCopy((map)d);
      break;
    case LIST_CMD:
      data= (void *)lCopy((lists)d);
      break;
    case LINK_CMD:
      data = (void *)slCopy((si_link) d);
      break;
    case RING_CMD:
    case QRING_CMD:
      {
        ring r=(ring)d;
        r->ref++;
        data=d;
        break;
      }
#ifdef TEST
    case DEF_CMD:
    case NONE:
      break; /* error recovery: do nothing */
    //case COMMAND:
    default:
      Warn("Copy: cannot copy type %s(%d)",Tok2Cmdname(rtyp),rtyp);
#endif
  }
  flag=source->flag;
  if ((source->attribute!=NULL)||(source->e!=NULL))
    attribute=source->CopyA();
  if (source->next!=NULL)
  {
    next=(leftv)Alloc(sizeof(sleftv));
    next->Copy(source->next);
  }
}

void * sleftv::CopyD(int t)
{
  if (iiCheckRing(t))
     return NULL;
  if ((rtyp!=IDHDL)&&(e==NULL))
  {
    void *x=data;
    if (rtyp==VNOETHER) x=(void *)pCopy(ppNoether);
    else if (rtyp==LIB_CMD)
      x=(void *)mstrdup((char *)Data());
    else if ((rtyp==VMINPOLY)&& (currRing->minpoly!=NULL)&&(currRing->ch<2))
      x=(void *)nCopy(currRing->minpoly);
    data=NULL;
    return x;
  }
  void *d=Data();
  if (!errorreported) return slInternalCopy(this,t,d,e);
  return NULL;
}

void * sleftv::CopyD()
{
  if ((rtyp!=IDHDL)&&(e==NULL)
  &&(rtyp!=VNOETHER)&&(rtyp!=LIB_CMD)&&(rtyp!=VMINPOLY))
  {
    void *x=data;
    data=NULL;
    return x;
  }
  return CopyD(Typ());
}

attr sleftv::CopyA()
{
  attr *a=Attribute();
  if ((a!=NULL) && (*a!=NULL))
    return (*a)->Copy();
  return NULL;  
}

char *  sleftv::String(void *d)
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
    return mstrdup("");
  }
#endif
  if (d==NULL) d=Data();
  if (!errorreported)
  {
    /* create a string, which may be freed by FreeL
    * leave the switch with return
    * or with break, which copies the string s*/
    char *s;
    const char *n;
    if (name!=NULL) n=name;
    else n=sNoName;
    switch (Typ())
    {
      case INT_CMD:
        s=(char *)AllocL(MAX_INT_LEN+2);
        sprintf(s,"%d",(int)d);
        return s;
      case STRING_CMD:
        return (char *)CopyD(STRING_CMD);
      case POLY_CMD:
      case VECTOR_CMD:
        s = pString((poly)d);
        break;
      case NUMBER_CMD:
        StringSetS("");
        if ((rtyp==IDHDL)&&(IDTYP((idhdl)data)==NUMBER_CMD))
        {
          nWrite(IDNUMBER((idhdl)data));
        }
        else if (rtyp==NUMBER_CMD)
        {
          number n=(number)data;
          nWrite(n);
          data=(char *)n;
        }
        else if((rtyp==VMINPOLY)&&(currRing->ch>2)&&(currRing->P==1))
        {
          nfShowMipo();
        }
        else
        {
          number n=nCopy((number)d);
          nWrite(n);
          nDelete(&n);
        }
        s = StringAppend("");
        break;
      case MATRIX_CMD:
        s= iiStringMatrix((matrix)d,2);
        break;
      case MODUL_CMD:
      case IDEAL_CMD:
      case MAP_CMD:
        s= iiStringMatrix((matrix)d,1);
        break;
      case INTVEC_CMD:
      case INTMAT_CMD:
      {
        intvec *v=(intvec *)d;
        return v->String();
      }
      case RING_CMD:
      case QRING_CMD:
      {
        return rString((ring)d);
      }  
      default:
        #ifdef TEST
        ::Print("String:unknown type %s(%d)", Tok2Cmdname(Typ()),Typ());
        #endif
        return NULL;
    } /* end switch: (Typ()) */
    return mstrdup(s);
  }
  return NULL;
}

int  sleftv::Typ()
{
  if (e==NULL)
  {
    switch (rtyp & 1023)
    {
      case IDHDL:
        return IDTYP((idhdl)data);
#ifdef SRING
      case VALTVARS:
#endif
      case VECHO:
      case VPAGELENGTH:
      case VPRINTLEVEL:
      case VCOLMAX:
      case VTIMER:
#ifdef HAVE_RTIMER
      case VRTIMER:
#endif          
      case VOICE:
      case VMAXDEG:
      case VMAXMULT:
      case TRACE:
      case VSHORTOUT:
        return INT_CMD;
      case LIB_CMD:
        return STRING_CMD;  
      case VMINPOLY:
        return NUMBER_CMD;
      case VNOETHER:
        return POLY_CMD;
      //case COMMAND:
      //  return COMMAND;
      default:
        return rtyp;
    }
  }
  int r=0;
  int t=rtyp;
  if (t==IDHDL) t=IDTYP((idhdl)data);
  switch (t)
  {
    case INTVEC_CMD:
    case INTMAT_CMD:
      r=INT_CMD;
      break;
    case IDEAL_CMD:
    case MATRIX_CMD:
    case MAP_CMD:
      r=POLY_CMD;
      break;
    case MODUL_CMD:
      r=VECTOR_CMD;
      break;
    case STRING_CMD:
      r=STRING_CMD;
      break;
    case LIST_CMD:
    {
      lists l;
      if (rtyp==IDHDL) l=IDLIST((idhdl)data);
      else             l=(lists)data;
      if ((0<e->start)&&(e->start<=l->nr+1))
      {
        l->m[e->start-1].e=e->next;
        r=l->m[e->start-1].Typ();
        l->m[e->start-1].e=NULL;
      }
      else
      {
        //Warn("out of range: %d not in 1..%d",e->start,l->nr+1);
        r=NONE;
      }
      break;
    }
    default:
      Werror("cannot index type %d",t);
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

void * sleftv::Data()
{
  if (iiCheckRing(rtyp))
     return NULL;
  if (e==NULL)
  {
    switch (rtyp)
    {
#ifdef SRING
      case VALTVARS:   return (void *)pAltVars;
#endif
      case VECHO:      return (void *)si_echo;
      case VPAGELENGTH:return (void *)pagelength;
      case VPRINTLEVEL:return (void *)printlevel;
      case VCOLMAX:    return (void *)colmax;
      case VTIMER:     return (void *)getTimer();
#ifdef HAVE_RTIMER
      case VRTIMER:    return (void *)getRTimer();
#endif
      case VOICE:      return (void *)(myynest+1);
      case VMAXDEG:    return (void *)Kstd1_deg;
      case VMAXMULT:   return (void *)Kstd1_mu;
      case TRACE:      return (void *)traceit;
      case VSHORTOUT:  return (void *)pShortOut;
      case VMINPOLY:   if ((currRing->minpoly!=NULL)&&(currRing->ch<2))
                       /* Q(a), Fp(a), but not GF(q) */
                         return (void *)currRing->minpoly;
                       else
                         return (void *)nNULL;
      case VNOETHER:   return (void *) ppNoether;
      case LIB_CMD:    {
                         idhdl h = ggetid( "LIB" );
                         if(h==NULL) return (void *)sNoName;
                         return IDSTRING(h);
                       }  
      case IDHDL:
        return IDDATA((idhdl)data);
      case COMMAND:
        //return NULL;
      default:
        return data;
    }
  }
  /* e != NULL : */
  int t=rtyp;
  void *d=data;
  if (t==IDHDL)
  {
    t=((idhdl)data)->typ;
    d=IDDATA((idhdl)data);
  }
  if (iiCheckRing(t))
    return NULL;
  char *r=NULL;
  switch (t)
  {
    case INTVEC_CMD:
    {
      intvec *iv=(intvec *)d;
      if ((e->start<1)||(e->start>iv->length()))
      {
        if (!errorreported)
          Werror("wrong range[%d] in intvec(%d)",e->start,iv->length());
      }
      else
        r=(char *)((*iv)[e->start-1]);
      break;
    }
    case INTMAT_CMD:
    {
      intvec *iv=(intvec *)d;
      if ((e->start<1)
         ||(e->start>iv->rows())
         ||(e->next->start<1)
         ||(e->next->start>iv->cols()))
      {
        if (!errorreported)
        Werror("wrong range[%d,%d] in intmat(%dx%d)",e->start,e->next->start,
                                                     iv->rows(),iv->cols());
      }
      else
        r=(char *)(IMATELEM((*iv),e->start,e->next->start));
      break;
    }
    case IDEAL_CMD:
    case MODUL_CMD:
    case MAP_CMD:
    {
      ideal I=(ideal)d;
      if ((e->start<1)||(e->start>IDELEMS(I)))
      {
        if (!errorreported)
          Werror("wrong range[%d] in ideal/module(%d)",e->start,IDELEMS(I));
      }
      else
        r=(char *)I->m[e->start-1];
      break;
    }
    case STRING_CMD:
    {
      r=(char *)AllocL(2);
      if ((e->start>0)&& (e->start<=(int)strlen((char *)d)))
      {
        r[0]=*(((char *)d)+e->start-1);
        r[1]='\0';
      }
      else
      {
        r[0]='\0';
      }
      break;
    }
    case MATRIX_CMD:
    {
      if ((e->start<1)
         ||(e->start>MATROWS((matrix)d))
         ||(e->next->start<1)
         ||(e->next->start>MATCOLS((matrix)d)))
      {
        if (!errorreported)
          Werror("wrong range[%d,%d] in intmat(%dx%d)",e->start,e->next->start,
                                                     MATROWS((matrix)d),MATCOLS((matrix)d));
      }
      else
        r=(char *)MATELEM((matrix)d,e->start,e->next->start);
      break;
    }
    case LIST_CMD:
    {
      lists l=(lists)d;
      int i=e->start-1;
      if ((0<=i)&&(i<=l->nr))
      {
        l->m[e->start-1].e=e->next;
        r=(char *)l->m[i].Data();
        l->m[e->start-1].e=NULL;
      }
      else //if (!errorreported)
        Werror("wrong range[%d] in list(%d)",e->start,l->nr+1);
      break;
    }
#ifdef TEST
    default:
      Werror("cannot index type %s(%d)",Tok2Cmdname(t),t);
#endif
  }
  return r;
}

attr * sleftv::Attribute()
{
  if (e==NULL) return &attribute;
  if ((rtyp==LIST_CMD)
  ||((rtyp==IDHDL)&&(IDTYP((idhdl)data)==LIST_CMD)))
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
        leftv r=l->m[e->start-1].LData();
        l->m[e->start-1].e=NULL;
        return r;
      }
      return &(l->m[e->start-1]);
    }
  }
  return this;
}

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

BOOLEAN assumeStdFlag(leftv h)
{
  if ((h->e!=NULL)&&(h->LTyp()==LIST_CMD))
  {
    return assumeStdFlag(h->LData());
  }
  if (!hasFlag(h,FLAG_STD))
  {
    if (!TEST_VERB_NSB)
      Warn("%s is no standardbasis",h->Name());
    return FALSE;
  }
  return TRUE;
}

/*2
* transforms a name (as an string created by AllocL or mstrdup)
* into an expression (sleftv), deletes the string
* utility for grammar and iparith
*/
extern BOOLEAN noringvars;
void syMake(leftv v,char * id)
{
  /* resolv an identifier: (to DEF_CMD, if siq>0)
  * 1) reserved id: done by scanner
  * 2) `basering`
  * 3) existing identifier, local
  * 4) ringvar, local ring
  * 5) existing identifier, global
  * 6) monom (resp. number), local ring: consisting of:
  * 6') ringvar, global ring
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
  memset(v,0,sizeof(sleftv));
#ifdef SIQ
  if (siq<=0)
#endif
  {
    idhdl h=NULL;
    if (!isdigit(id[0]))
    {
      if (strcmp(id,"basering")==0)
      {
        if (currRingHdl!=NULL)
        {
          if (id!=IDID(currRingHdl)) FreeL((ADDRESS)id);
          v->rtyp = IDHDL;
          v->data = (char *)currRingHdl;
          v->name = IDID(currRingHdl);
          v->flag = IDFLAG(currRingHdl);
          return;
        }
        else
        {
          v->name = id;
          return; /* undefined */
        }
      }
      h=ggetid(id);
      /* 3) existing identifier, local */
      if ((h!=NULL) && (IDLEV(h)==myynest))
      {
        if (id!=IDID(h)) FreeL((ADDRESS)id);
        v->rtyp = IDHDL;
        v->data = (char *)h;
        v->flag = IDFLAG(h);
        v->name = IDID(h);
        v->attribute=IDATTR(h);
        return;
      }
    }
    /* 4. local ring: ringvar */
    if ((currRingHdl!=NULL) && (IDLEV(currRingHdl)==myynest))
    {
      int vnr;
      if ((vnr=rIsRingVar(id))>=0)
      {
        poly p=pOne();
        pSetExp(p,vnr+1,1);
        pSetm(p);
        v->data = (void *)p;
        v->name = id;
        v->rtyp = POLY_CMD;
        return;
      }
    }
    /* 5. existing identifier, global */
    if (h!=NULL)
    {
      if (id!=IDID(h)) FreeL((ADDRESS)id);
      v->rtyp = IDHDL;
      v->data = (char *)h;
      v->flag = IDFLAG(h);
      v->name = IDID(h);
      v->attribute=IDATTR(h);
      return;
    }
    /* 6. local ring: number/poly */
    if ((currRingHdl!=NULL) && (IDLEV(currRingHdl)==myynest))
    {
      BOOLEAN ok=FALSE;
      poly p = (!noringvars) ? pmInit(id,ok) : (poly)NULL;
      if (ok)
      {
        if (p==NULL)
        {
          v->data = (void *)nInit(0);
          v->rtyp = NUMBER_CMD;
          FreeL((ADDRESS)id);
        }
        else
        if (pIsConstant(p))
        {
          v->data = pGetCoeff(p);
          pGetCoeff(p)=NULL;
          pFree1(p);
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
    /* 7. non-local ring: number/poly */
    {
      BOOLEAN ok=FALSE;
      poly p = ((currRingHdl!=NULL)&&(!noringvars)&&(IDLEV(currRingHdl)!=myynest))
               /* ring required */  /* not in decl */    /* already in case 4/6 */
                     ? pmInit(id,ok) : (poly)NULL;
      if (ok)
      {
        if (p==NULL)
        {
          v->data = (void *)nInit(0);
          v->rtyp = NUMBER_CMD;
          FreeL((ADDRESS)id);
        }
        else
        if (pIsConstant(p))
        {
          v->data = pGetCoeff(p);
          pGetCoeff(p)=NULL;
          pFree1(p);
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
    /* 8. basering ? */
    if ((myynest>1)&&(currRingHdl!=NULL))
    {
      if (strcmp(id,IDID(currRingHdl))==0)
      {
        if (IDID(currRingHdl)!=id) FreeL((ADDRESS)id);
        v->rtyp=IDHDL;
        v->data=currRingHdl;
        v->name=IDID(currRingHdl);
        v->attribute=IDATTR(currRingHdl);
        return;
      }
    }
  }
#ifdef SIQ
  else
    v->rtyp=DEF_CMD;
#endif
  /* 9: _ */
  if (strcmp(id,"_")==0)
  {
    FreeL((ADDRESS)id);
    v->Copy(&sLastPrinted);
  }
  else
  {
    /* 10: everything else */
    /* v->rtyp = UNKNOWN;*/
    v->name = id;
  }
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
          leftv r=iiMake_proc(h,&d->arg2);
          if (r!=NULL)
            memcpy(this,r,sizeof(sleftv));
          else
            nok=TRUE;
        }
      }
      else nok=TRUE;
    }
    else if (d->op=='=') //assume d->argc==2
    {
      char *n=d->arg1.name;
      if (n!=NULL)
      {
        nok=d->arg2.Eval();
        if (!nok)
        {
          int save_typ=d->arg1.rtyp;
          mmTestLP(n);
          syMake(&d->arg1,n); //assume  type of arg1==DEF_CMD
          mmTestLP(d->arg1.name);
          if (d->arg1.rtyp==IDHDL)
          {
            n=mstrdup(IDID((idhdl)d->arg1.data));
            killhdl((idhdl)d->arg1.data);
            d->arg1.data=NULL;
            d->arg1.name=n;
          }
          //d->arg1.rtyp=DEF_CMD;
          sleftv t;
          if(save_typ!=PROC_CMD) save_typ=d->arg2.rtyp;
          if ((BEGIN_RING<d->arg2.rtyp)&&(d->arg2.rtyp<END_RING)
          /*&&(QRING_CMD!=d->arg2.rtyp)*/)
            nok=iiDeclCommand(&t,&d->arg1,0,save_typ,&currRing->idroot);
          else
            nok=iiDeclCommand(&t,&d->arg1,0,save_typ,&idroot);
          memcpy(&d->arg1,&t,sizeof(sleftv));
          mmTestLP(d->arg1.name);
          nok=nok||iiAssign(&d->arg1,&d->arg2);
          mmTestLP(d->arg1.name);
          if (!nok)
          {
            memset(&d->arg1,0,sizeof(sleftv));
            this->CleanUp();
            memset(this,0,sizeof(sleftv));
            rtyp=NONE;
          }
        }
      }
      else nok=TRUE;
    }
    else if (d->argc==1)
    {
      nok=d->arg1.Eval();
      nok=nok||iiExprArith1(this,&d->arg1,d->op);
    }
    else if(d->argc==2)
    {
      nok=d->arg1.Eval();
      nok=nok||d->arg2.Eval();
      nok=nok||iiExprArith2(this,&d->arg1,d->op,&d->arg2);
    }
    else if(d->argc==3)
    {
      nok=d->arg1.Eval();
      nok=nok||d->arg2.Eval();
      nok=nok||d->arg3.Eval();
      nok=nok||iiExprArith3(this,d->op,&d->arg1,&d->arg2,&d->arg3);
    }
    else if(d->argc!=0)
    {
      nok=d->arg1.Eval();
      nok=nok||iiExprArithM(this,&d->arg1,d->op);
    }
    else // d->argc == 0
    {
      nok = iiExprArithM(this, NULL, d->op);
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
    case POLY_CMD:
      pTest((poly)Data());
      break;
    case IDEAL_CMD:
    case MODUL_CMD:
    case MATRIX_CMD:
      {
        ideal id=(ideal)Data();
        mmTest(id,sizeof(*id));
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

