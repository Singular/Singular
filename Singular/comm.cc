#include "mod2.h"

#include <string.h>
//#include <sys/socket.h>
//#include <sys/types.h>
//#include <netinet/in.h>
//#include <unistd.h>
//#include <signal.h>
#include <stdio.h>
#include "mod2.h"

#ifdef STDTRACE

#include "mmemory.h"
#include "polys.h"
#include "febase.h"
#include "kutil.h"
#include "kstd1.h"
#include "kstd2.h"
#include "khstd.h"
#include "spolys.h"
#include "stairc.h"
#include "weight.h"
#include "cntrlc.h"
#include "intvec.h"
#include "ideals.h"
#include "ipshell.h"
#include "ipid.h"
#include "timer.h"
#include "tok.h"
#include "lists.h"

#include "mpsr.h"
#include "ring.h"
#include "maps.h"
#include "comm.h"
#include "timer.h"

#define DEBUG 0

void skstdLink::Kill()
{
//   PrintS("Kill\n");
  if(DEBUG) PrintS("Kill\n");
  if(msg!=NULL)
    delete(msg);
  if(links!=NULL)
    delete(links);
  if(res!=NULL)
    {
      res->~skintarray();
      delete(res);
    }
    
  Free((ADDRESS)Warteliste,(Wartelistemax)*sizeof(LObject));
}

//###################################################################

void skstdLink::Init(leftv lv, int rw)
{
//   PrintS("Init\n");
  si_link l;
  leftv ptr = lv;
  int i;
  procnum =0;
  for(i=0;ptr->next!=NULL;i++,procnum++)
    ptr=ptr->next;
  procnum++;

  msg    = new intvec(procnum,1,0);
  links  = new sklinkvec(procnum);
  res    = new skintarray(procnum);

  if(rw==0)
    {
      lnModus=lnRead;
      Modus = ModRead;
    }
  else
    {
      lnModus=lnWrite;
      Modus = ModWrite;
    }

  //++++++++++++++++++++++++++++++++++++++++++++++++++++
  // Links kopieren
  //++++++++++++++++++++++++++++++++++++++++++++++++++++
  ptr = lv;
  for(i=0; i<procnum ; i++)
    {
      (*links)[i] = (si_link) ptr->Data();
      ptr=ptr->next;
    }
  //++++++++++++++++++++++++++++++++++++++++++++++++++++
  // Verschiedenes initialisieren
  //++++++++++++++++++++++++++++++++++++++++++++++++++++

  Verwaltung  = FALSE;
  Receive     = TRUE;
  TupelL      = FALSE;
  SPoly       = FALSE;
  Reduzieren  = FALSE;
  TupelStore  = FALSE;
  TupelMelden = FALSE;
  TupelTesten = FALSE;
  TupelPosition = FALSE;
  ResultSend  = FALSE;

  //++++++++++++++++++++++++++++++++++++++++++++++++++++
  // Warteliste für Tupel initialisieren
  //++++++++++++++++++++++++++++++++++++++++++++++++++++
  if(lnModus==lnRead)
    {
      Warteliste = initL();
      Wartelistemax = 16;
      Wartelistel=0;
    }
}

//###################################################################

void skstdLink::Store(LObject P)
{
//   PrintS("Store\n");
  if(DEBUG) PrintS("Store\n");
  enterL(&Warteliste,&Wartelistel,&Wartelistemax,P ,aktuell++);
  P.p=NULL;
}

//###################################################################

BOOLEAN skstdLink::Restore(kStrategy strat)
{
//   PrintS("Restore\n");
  if(DEBUG) PrintS("Restore\n");
  
  int i = 0;
  int l = 0;
  if(aktuell>0)
    {
      while(i<aktuell)
	{
	  l = strat->posInL(strat->L,strat->Ll,Warteliste[i],strat);
	  enterL(&(strat->L),&(strat->Ll),&(strat->Lmax),Warteliste[i++],l);
	}
      
      aktuell=0;
      return TRUE;
    }
  else
    return FALSE;
}

//###################################################################

BOOLEAN skstdLink::QueryRestore()
{
//   PrintS("QueryRestore\n");
  if(DEBUG) PrintS("QueryRestore\n");

 return ((aktuell>0) || (links==NULL));
}

//###################################################################

void skstdLink::SendMsg(kStrategy strat, stdMsg msg,int li)
{
//   PrintS("SendMsg\n");
  int i;
  if(lnModus == lnWrite)
    {
      if(DEBUG) PrintS("SendMsg ");

      int pos=0;
      intvec exp(pVariables+1, 1, 0);
      exp[0]=msg;
      leftv lv = (leftv) Alloc0(sizeof(sleftv));
      lv->next=NULL;
      lv->rtyp = INTVEC_CMD;
      switch(msg)
	{

	case MsgTupel:
	  for(i=0; i<pVariables; i++)
	    exp[i+1]=pGetExp(strat->P.p, i+1);
	  break;

	case MsgTupelPosition:
	  for(i=0; i<pVariables; i++)
	    exp[i+1]=0;

	  // Neue Position von strat->P.p bestimmen
	  pos=-1;
	  for(i=strat->Ll-1; i>=0; i--)
	    {
	      if((strat->L[i].p1 == strat->P.p1) && (strat->L[i].p2 == strat->P.p2))
		{ 
		  pos = i;
		  continue;
		}
	    }
	  exp[1]=pos;
	  break;

	default:
	  for(i=0; i<pVariables; i++)
	    exp[i+1]=0;
	  break;
	}
      lv->data=ivCopy(&exp);
      if(li==-1)
	{
	  for(i=0; i< procnum; i++)
	    if((*links)[i]!=NULL)
	      slWrite((*links)[i], lv);
	}
      else
	{
	  slWrite((*links)[li], lv);
	}
      lv->CleanUp();
      Free(lv,sizeof(sleftv));
      if(DEBUG) PrintS(" Ende\n");
    }
}

//###################################################################

BOOLEAN skstdLink::ReceiveMsg()
{
//   PrintS("ReceiveMsg\n");
  if(lnModus==lnRead)
    {
      intvec *rec = NULL;
      if(DEBUG) PrintS("ReceiveMsg\n");
  
      leftv lv = NULL;
      for(int i=0; i<procnum;i++)
	{
	  lv=slRead((*links)[i]);
	  rec=ivCopy((intvec *) lv->Data());
 	  if((*res)[i]!=NULL)
 	    {
 	      delete (*res)[i];
 	      (*res)[i]=NULL;
 	    }
	  (*msg)[i] = (*rec)[0];
	  if((stdMsg)(*msg)[i]==MsgTupel)
	    {
	      (*res)[i] = new intvec(pVariables,1,0);
	      for(int j = 0; j<pVariables ; j++)
		(*(*res)[i])[j]=(*rec)[j+1];
	    }
	}
        lv->CleanUp();
        Free(lv, sizeof(sleftv));

      delete rec;
      if(DEBUG) PrintS("ReceiveMsg Ende\n");
      return TRUE;  
    }
}

//###################################################################

void skstdLink::SetModus(stdModus NeuerModus, kStrategy strat=NULL)
{
 //  PrintS("SetModus\n");
  if(DEBUG) PrintS("SetModus\n");
  Modus=NeuerModus;
  switch(Modus)
    {
    case ModRead:  // Warte auf erste Nachricht
       Verwaltung  = FALSE;
       Receive     = TRUE;
       TupelL      = FALSE;
       SPoly       = FALSE;
       Reduzieren  = FALSE;
       TupelMelden = FALSE;
       TupelTesten = FALSE;
       TupelPosition = FALSE;
       ResultSend  = FALSE;
       TupelStore  = FALSE;
       break;
    case ModWrite:
    case ModRun:  // Berechne und sende Tupel
      Verwaltung  = TRUE;     
      Receive     = FALSE;    
      TupelL      = TRUE;     
      SPoly       = TRUE;     
      Reduzieren  = TRUE;     
      TupelMelden = TRUE;     
      TupelTesten = FALSE;    
      TupelPosition = TRUE;
      ResultSend  = FALSE;    
      TupelStore  = FALSE;    
      break;
    case ModStep:  // Warte auf MsgStep, berechne und sende
      Verwaltung  = TRUE;     
      Receive     = TRUE;    
      TupelL      = TRUE;     
      SPoly       = TRUE;     
      Reduzieren  = TRUE;     
      TupelMelden = TRUE;     
      TupelPosition = TRUE;
      TupelTesten = FALSE;    
      ResultSend  = TRUE;    
      TupelStore  = FALSE;    
      break;
    case ModCalc: // Warte auf Tupel und berechne
      Verwaltung  = TRUE; 
      Receive     = TRUE;    
      TupelL      = TRUE;     
      TupelMelden = FALSE;    
      TupelPosition = FALSE;
      ResultSend  = TRUE;
      TupelStore  = (*msg)[0]==MsgTupelisZero;
      if(TupelStore)
	{
 	  SPoly      = FALSE; 
 	  Reduzieren = FALSE;
 	  TupelTesten = FALSE;
	}
      else
	{
	  SPoly      = TRUE;  
	  Reduzieren = TRUE;  
	  TupelTesten = TRUE;
	}
      break;
    case ModPosition: // Tupel neu positionieren
      Verwaltung  = TRUE; 
      Receive     = TRUE;    
      TupelL      = TRUE;     
      TupelMelden = FALSE;    
      TupelPosition = FALSE;
      ResultSend  = TRUE;
      TupelStore  = FALSE;
      SPoly       = TRUE;
      Reduzieren  = TRUE;
      TupelTesten = FALSE;
      break;
    case ModCheck: // Nachrechnen
      Restore(strat);
      if(strat->Ll>=0)
	{
	  Verwaltung  = TRUE;
	  Receive     = FALSE;
	  TupelL      = TRUE;
	  TupelStore  = FALSE;    
	  SPoly       = TRUE;
	  TupelMelden = FALSE;
	  TupelPosition = FALSE;
	  TupelTesten = FALSE;
	  ResultSend  = TRUE;
	  Reduzieren  = TRUE;
	}
      else
	{
	  SetModus(ModEnde,strat);
	}
      break;
    case ModEnde:  //sofort beenden
      Verwaltung  = FALSE;
      Receive     = FALSE;    
      TupelWarte  = FALSE;    
      TupelL      = FALSE;    
      TupelStore  = FALSE;    
      TupelTesten = FALSE;    
      SPoly       = FALSE;    
      Reduzieren  = FALSE;    
      TupelMelden = FALSE;    
      ResultSend  = FALSE;    
      break;
    }
  if(DEBUG) PrintS("SetModus Ende\n");
}

//###################################################################

BOOLEAN skstdLink::CheckEnd(kStrategy strat)
{
//   PrintS("CheckEnd\n");
  if(DEBUG) PrintS("CheckEnd\n");
  switch(Modus)
    {
    case ModRun:    // Normales beenden
      SetModus(ModRun);
      return strat->Ll<0;
      break;
    case ModStep:    // Normales beenden
      SetModus(ModStep);
      return strat->Ll<0;
      break;
    case ModCalc:   // Beenden erst dann, wenn Calc2 erreicht ist.
      SetModus(ModCalc);
      if(strat->Ll<0) 
	{
	  if(BTEST1(OPT_INTERRUPT))
	    return TRUE;
	  if(TEST_OPT_PROT)
            {
	      PrintS("\n<C>\n");
              writeTime("used time: #");
            }
 	  SetModus(ModCheck,strat);
	}
	return FALSE;
      break;
    case ModCheck:
      SetModus(ModCheck,strat);
      return strat->Ll<0;
    case ModEnde:
      SetModus(ModEnde,strat);
      return TRUE;
      break;
    case ModRead:
      SetModus(ModCalc,strat);
      return FALSE;
      break;
    case ModWrite:
      SetModus(ModRun,strat);
      return FALSE;
      break;
    }
  return FALSE;
}

//###################################################################

void skstdLink::CheckPosition(kStrategy strat, int oldLl)
{
//   PrintS("CheckPosition\n");
  if(lnModus == lnWrite)
    {
      if(strat->Ll != oldLl )
	{
	  SendMsg(strat, MsgTupelPosition);
	  TupelMelden = FALSE;
	}
    }
  
}

//###################################################################

void skstdLink::CheckHilb(kStrategy strat, int oldLl)
{
//   PrintS("CheckHilb\n");
  if(strat->Ll < oldLl)
    for(int i= oldLl - strat->Ll; i>0 ; i--)
      SendMsg(strat, MsgTupelHilb);
}

//###################################################################

void skstdLink::ParseMessage(kStrategy strat)
{
//   PrintS("ParseMessage\n");
  if(DEBUG) PrintS("ParseMessage\n");
  BOOLEAN err=FALSE; 
  // Testen, ob alle Nachrichten korrekt sind,
  // wenn ja, dann ParseMessageFinal
  // sonst die richtige finden, andere Prozesse killen
  
  // Ist nur ein proc vorhanden, dann automatisch weiter
  for(int i=1; i< procnum; i++)  
    err = err || ((*msg)[i] != (*msg)[0]);
  if(!err)
    ParseMessageFinal(strat);
  else
    {
      PrintS("Unterschiedliche Messages !\n");
      // Mögliche Kombinationen
      // TupelNull + Tupel      : Alle "TupelNull" löschen
      // Unterschiedliche Tupel : In TupelDifferent alle abweichler killen
      for(int i=0; i<procnum; i++)
	if(((stdMsg)(*msg)[i]) == MsgTupelisZero)
	  {
	    PrintS(" AAA ");
 	    KillChild(strat,i);
	  }
      ParseMessageFinal(strat);
    }
  if(DEBUG) PrintS("ParseMessage Ende\n");
}

//###################################################################

void skstdLink::ParseMessageFinal(kStrategy strat, int i)
{
//   PrintS("ParseMessageFinal\n");
  if(DEBUG) PrintS("ParseMessageFinal\n");
  
  switch(((stdMsg) (*msg)[i]))
    {
    case MsgTupel:  // In Step-Modus schalten bzw. bleiben.     
    case MsgTupelisZero:

       if(procnum>0)
 	{
 	  SetModus(ModCalc,strat);
 	}
       else
 	{
 	  SetModus(ModCheck,strat);
 	}
      break;
    case MsgTupelPosition:
      // aktuelles Tupel neu positionieren
      if(TEST_OPT_PROT)
	PrintS("P");
      SetModus(ModPosition);
      break;
     case MsgEnd:  // In Calc1-Modus schalten bzw. bleiben.     
       SetModus(ModCheck,strat);
       if (TEST_OPT_PROT) PrintS("\n<E>");
       break;
    default:
      break;
    }
  if(DEBUG) PrintS("ParseMessageFinal Ende\n");
}

//###################################################################

void skstdLink::TupelDifferent(kStrategy strat)
{
//   PrintS("TupelDifferen\n");
  if(DEBUG) PrintS("TupelDifferent\n");
  BOOLEAN Different=FALSE ;
  for(int i=procnum-1; i>=0; i--)
    {
      if(strat->P.p==NULL)
	{
	  PrintS(" BBB ");
 	  KillChild(strat,i);
	}
      else
	{
	  if((*res)[i]==NULL)
	    {
	      PrintS(" CCC ");
 	      KillChild(strat,i);
	    }
	  else
	    {
	      Different = FALSE;
	      for(int j=0; j< pVariables; j++)
		{
		  Different = Different || ((*(*res)[i])[j]!=pGetExp(strat->P.p,j+1));
		}
	      if(Different)
		{
		  PrintS(" DDD ");
 		  KillChild(strat,i);
		}
	    }
	}
    }
  if(procnum<=0)
    SetModus(ModCheck,strat);
}

//###################################################################

void skstdLink::SendTupel(kStrategy strat)
{
//   PrintS("SendTupel\n");
  if(DEBUG) PrintS("SendTupel\n");
  if(strat->P.p==NULL)
    {
      SendMsg(strat,MsgTupelisZero);
    }
  else
      SendMsg(strat,MsgTupel);
}

//###################################################################

void skstdLink::KillChild(kStrategy strat, int li,BOOLEAN Cancled)
{
  //    PrintS("KillChild\n");
  if(DEBUG) PrintS("KillChild\n");
  if(li>=0)
     {
       if(procnum>=2)
 	{
  	  si_link swaplink = (*links)[li];
  	  intvec *swapres = (*res)[li];
  	  (*msg)[li]   = (*msg)[procnum-1];
  	  (*links)[li] = (*links)[procnum-1];
  	  (*res)[li]   = (*res)[procnum-1];
  	  (*links)[procnum-1] = swaplink;
  	  (*res)[procnum-1]   = swapres;
 	}
        else
	  {
	    if(TEST_OPT_PROT)
	      {
		PrintS("\n<K>\n");
		writeTime("used time: #");
	      }
	    SetModus(ModCheck,strat);
	  }
       procnum--;
     }
   else
     {
       for(int i=procnum - 1 ; i>=0 ; i--)
 	KillChild(strat,i,Cancled);
     }
 }

//###################################################################

void skstdLink::Start(kStrategy strat)
{
    if(lnModus==lnRead)
      {
	SetModus(ModCalc);
      }
    else
      {
	SetModus(ModRun  );
      }
}

//###################################################################

void skstdLink::End(kStrategy strat)
{
  if(DEBUG) PrintS("End\n");
}

//###################################################################

lists skstdLink::RestTupel()
{
  int i=0;
  lists l=(lists)Alloc(sizeof(slists));
  ideal I;
  if(aktuell>0)
    {
      l->Init(aktuell);
      for(i=0; i<aktuell; i++)
	{
 	  I=idInit(2,1);
	  l->m[i].rtyp = IDEAL_CMD;
	  if(Warteliste[i].p1==NULL)
	    I->m[0]=NULL;
	  else
	    I->m[0]=pCopy(Warteliste[i].p1);
	  if(Warteliste[i].p2==NULL)
	    I->m[1]=NULL;
	  else
	    I->m[1]=pCopy(Warteliste[i].p2);
 	  l->m[i].data = (void *) idCopy(I);
  	  idDelete(&I);
	}
      return l;
    }
  else
    {
      l->Init(0);
      return l;
    }
  
}

//###################################################################

void skstdLink::DispMsg(stdMsg msg)
{
    {
//       Print("r(%.5i) Msg =",-1);
      switch(msg) {
      case MsgTupelisZero:  // In Step-Modus schalten bzw. bleiben.     
	PrintS("MsgTupelisZero ");
	break;
      case MsgTupel:  // In Step-Modus schalten bzw. bleiben.     
	PrintS("MsgTupel       ");
	break;
      case MsgEnd:
	PrintS("MsgEnd         ");
	break;
      }
    }
}
 
void skstdLink::DispMod(int i)
{
  if(i==-1 )
    {
      Print("r(%.5i) Mod = ",-1);
      
      switch(Modus) {
      case ModCheck:
	PrintS("Check ");
	break;
      case ModRun:
	PrintS("Run   ");
	break;
      case ModStep:
	PrintS("Step  ");
	break;
      case ModCalc:
	PrintS("Calc  ");
	break;
      case ModRead:
	PrintS("Read ");
	break;
      case ModWrite:
	PrintS("Write  ");
	break;
      }
      PrintS("\n");
    }
}
#endif
