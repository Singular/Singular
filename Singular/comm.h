#ifndef COMM_H
#define COMM_H

#include <sys/types.h>
#include "silink.h"
#include "intvec.h"
#include "structs.h"
//#include "febase.h"
#include "structs.h"
#include "kutil.h"

#include <stdio.h>

extern int pVariables;

enum stdModus
{
  ModEnde,          // Berechnung sofort verlassen
  ModRead,          // Nur zur besseren Lesbarkeit
  ModWrite,         //  "   "     "         "
  ModRun,           // Normales rechnen, Tupel versenden
  ModStep,          // Tupel Empfangen, Rechenweg richtet sich nach Nachricht
  ModCalc,          // Normale Rechnen Tupel nicht versenden
  ModCheck,         // Nachrechnen
  ModPosition       // Aktuelles Tupel wird beim Reduzieren
                    // neu in der Liste strat->L positioniert.
};

enum linkModus
{
  lnRead,
  lnWrite
};


enum stdMsg
{
                         // Wird gesendet wenn :
  MsgEnd           = 2,  // schreibender Prozess die Berechnung beendet hat.
  MsgTupelisZero   = 3,  // wenn aktuelles Tupel zu NULL geht
  MsgTupel         = 4,  // wenn aktuelles Tupel nicht zu NULL geht
  MsgTupelPosition = 5,  // wenn aktuelles Tupel in strat->Ll neu positioniert wird
  MsgTupelHilb     = 6   // wenn ein Tupel mit Hilberfunktion elimiert wurde.
};


class sklinkvec;
typedef class sklinkvec * linkvec;
class sklinkvec
{
private:
  si_link * link;
public:
  int nr;
  sklinkvec(int l=1) {
    nr=l;
    link = (si_link *)Alloc0(sizeof(sip_link)*l);
  }

  si_link& operator[](int i) {
    return link[i];
  }
};

class skintarray;
typedef class skintarray * intarray;
class skintarray
{
private:
   intvec * * vec;
public:
  int nr;
  skintarray(int l=1) {
    nr=l;
     vec = (intvec **)Alloc0(sizeof(intvec*)*l);
    for(int i=0; i<l ; i++)
      vec[i] = NULL;
  }

  intvec*& operator[](int i) {
    return vec[i];
  }

   ~skintarray() {
     if(vec!=NULL)
       {
          for(int i=0; i<nr; i++)
           {
             if((vec[i])!=NULL)
               delete (vec[i]);
             vec[i]=NULL;
           }
       }
     Free((ADDRESS)vec,sizeof(intvec*)*nr);
     vec=NULL;
   }
};



class skstdLink;
typedef class skstdLink * stdLink;

class skstdLink
{
public:
  stdModus Modus;
  BOOLEAN Verwaltung,
          Receive,
          TupelL,
          TupelWarte,
          SPoly,
          Reduzieren,
          TupelStore,
          TupelMelden,
          TupelTesten,
          TupelPosition,
          ResultSend;

  void Init(leftv ,int);
  void Start(kStrategy);

  void Store(LObject);           // Speichert ein Polynom in der Liste l zur späteren Verarbeitung
  BOOLEAN Restore(kStrategy);    // Holt das ein in l gespeicherten Polynom zur Verarbeitung heran.

  BOOLEAN ReceiveMsg();
  void SendTupel(kStrategy);
  void ParseMessage(kStrategy);
  void TupelDifferent(kStrategy);
  void CheckPosition(kStrategy, int);
  void CheckHilb(kStrategy, int);

  void Kill();

  BOOLEAN CheckEnd(kStrategy);
  void End(kStrategy);
  lists RestTupel();

  void DispMsg(stdMsg);
  void DispMod(int i=-1);
private:
  //  stdModus Modus;
  linkModus lnModus;

  LSet Warteliste;
  int Wartelistemax, Wartelistel;
  int aktuell;         // Index des Polynoms in l, das als nächstes verarbeitet wird.

  int      procnum;
  intarray res;
  linkvec  links;
  intvec * msg;

  void ParseMessageFinal(kStrategy, int=0);
  void SendMsg(kStrategy, stdMsg msg,int i =0);
  BOOLEAN QueryRestore();
  void SetModus(stdModus,kStrategy);
  void KillChild(kStrategy,int=-1,BOOLEAN=TRUE);
};


#endif
