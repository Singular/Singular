module="pcv";

version="$Id: pcv.mod,v 1.3 1999-03-31 22:08:13 krueger Exp $";
info="
LIBRARY: pcv.so  CONVERSION BETWEEN POLYS AND COEF VECTORS
AUTHOR:  Mathias Schulze, email: mschulze@mathematik.uni-kl.de

 MinDeg(p);      min deg of monomials of poly p
 MaxDeg(p);      max deg of monomials of poly p
 P2CV(l,d0,d1);  list of coef vectors from deg d0 to d1 of polys in list l
 CV2P(l,d0,d1);  list of polys with coef vectors from deg d0 to d1 in list l
 Dim(d0,d1);     number of monomials from deg d0 to d1
 Basis(d0,d1);   list of monomials from deg d0 to d1
";

cxxsource = pcv/pcv.cc

proc int MinDeg(poly) {
   function=pcvMinDeg;
}

proc int MaxDeg(poly) {
   function=pcvMaxDeg;
}

proc list P2CV(list,int,int) {
C={ 
  /* check if current RingHandle is set */
  if(currRingHdl == NULL)
  {
    WerrorS("no ring active");
    return TRUE;
  }
};
function=pcvP2CV;
}

proc list CV2P(list,int,int)
{
C={  /* check if current RingHandle is set */
  if(currRingHdl == NULL)
  {
    WerrorS("no ring active");
    return TRUE;
  }
};
function=pcvCV2P;}

proc int Dim(int,int)
{
C = {
  /* check if current RingHandle is set */
  if(currRingHdl == NULL)
  {
    WerrorS("no ring active");
    return TRUE;
  }
};
function=pcvDim;

}

proc list Basis(int,int)
{
C = {
  /* check if current RingHandle is set */
  if(currRingHdl == NULL)
  {
    WerrorS("no ring active");
    return TRUE;
  }
};
function=pcvBasis;}

