%{
/*
 *  $Id$
 *
 *  Test mod fuer modgen
 */

#include <stdio.h>
#include "Pcv.h"
%}


// module="pcv";
package="pcv";

version="$Id$";
info="
LIBRARY: pcv.so  CONVERSION BETWEEN POLYS AND COEF VECTORS
AUTHOR:  Mathias Schulze, email: mschulze@mathematik.uni-kl.de

 MinDeg(p);      min deg of monomials of poly p
 P2CV(l,d0,d1);  list of coef vectors from deg d0 to d1 of polys in list l
 CV2P(l,d0,d1);  list of polys with coef vectors from deg d0 to d1 in list l
 Dim(d0,d1);     number of monomials from deg d0 to d1
 Basis(d0,d1);   list of monomials from deg d0 to d1
";

cxxsource = Pcv.cc;

%procedures


int MinDeg(poly p) {
   %declaration;
   %typecheck;
   %return(pcvMinDeg(p));
}

list P2CV(list pl,int d0,int d1) {
  %declaration;

  /* check if current RingHandle is set */
  if(currRingHdl == NULL)
  {
    WerrorS("no ring active");
    return TRUE;
  }
  
  %typecheck;
  %return(pcvP2CV(pl, d0, d1));
}

list CV2P(list pl,int d0,int d1)
{
  %declaration;

  /* check if current RingHandle is set */
  if(currRingHdl == NULL)
  {
    WerrorS("no ring active");
    return TRUE;
  }
  
  %typecheck;
  %return(pcvCV2P(pl, d0, d1));
}

int Dim(int d0,int d1)
{
  %declaration;

  /* check if current RingHandle is set */
  if(currRingHdl == NULL)
  {
    WerrorS("no ring active");
    return TRUE;
  }
  
  %typecheck;
  %return(pcvDim);
}

list Basis(int d0,int d1)
{
  %declaration;

  /* check if current RingHandle is set */
  if(currRingHdl == NULL)
  {
    WerrorS("no ring active");
    return TRUE;
  }
  
  %typecheck;
  %return(pcvBasis);
}

// %%

