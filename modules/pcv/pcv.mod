module="pcv";

version="$Id: pcv.mod,v 1.1 1998-12-17 11:59:42 mschulze Exp $";
info="
LIBRARY: pcv.so  CONVERSION BETWEEN POLYS AND COEF VECTORS
                 by Mathias Schulze
                 email: mschulze@mathematik.uni-kl.de

 MinDeg(p);      min deg of monomials of poly p
 MaxDeg(p);      max deg of monomials of poly p
 P2CV(l,d0,d1);  list of coef vectors from deg d0 to d1 of polys in list l
 CV2P(l,d0,d1);  list of polys with coef vectors from deg d0 to d1 in list l
 Dim(d0,d1);     number of monomials from deg d0 to d1
 Basis(d0,d1);   list of monomials from deg d0 to d1
";

cxxsource = pcv/pcv.cc

proc MinDeg(poly) = pcvMinDeg;
proc MaxDeg(poly) = pcvMaxDeg;
proc P2CV(list,int,int) = pcvP2CV;
proc CV2P(list,int,int) = pcvCV2P;
proc Dim(int,int) = pcvDim;
proc Basis(int,int) = pcvBasis;
