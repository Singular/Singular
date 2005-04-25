LIB "tst.lib";
tst_init();
LIB "equising.lib";
///////////////////////////////////////////////////////////////////////////////
//                   Baby Examples
ring r=0,(x,y),ds;
poly f= x2-y3;
esIdeal(f);
ring R=0,(A,B,C,D,E,F,x,y),ds;
poly F=x2-y3+Ax+By+Cy2+Dxy+Ex2+Fy3;
esStratum(F,2);
esStratum(F,3);
kill r;
//
ring rr=0,(x,y),ls;
poly f=x7+y7+(x-y)^2*x2y2+x2y4; // Newton non-degenerate
tau_es(f);
list K=esIdeal(f);
vdim(std(K[1])); // tau_es
vdim(std(K[2])); // tau_es_fix 
// 
f=x7+y7+(x-y)^2*x2y2; // Newton degenerate
tau_es(f);
K=esIdeal(f);
vdim(std(K[1])); 
vdim(std(K[2])); 
//
f=x6y-3x4y4-x4y5+3x2y7-x4y6+2x2y8-y10+2x2y9-y11+x2y10-y12-y13;  // 4 branches
tau_es(f);        
K=esIdeal(f);
vdim(std(K[1]));
K;
kill rr;

///////////////////////////////////////////////////////////////////////////////
//                  Teste Verhalten in verschiedenen Ringen 
proc tst(list d)
{
 int i;
 for(i=1;i<=size(d);i++)
   {
     execute(d[i]);
     
     poly f = y8+4x3y6+6x6y4+2x5y5+4x9y2+4x8y3+x12+2x11y+2x10y2+x13;
               	    
     list L=versal(f);
     def Px=L[1];
     setring Px;
     poly F = Fs[1,1];
              
     list EsList = esStratum(F);
     EsList[2];  
     if (typeof(EsList[1])=="list") 
     { 
       ideal ES = EsList[1][1]; kill(EsList);
     }
     else
     {
       def R=EsList[1]; kill(EsList); setring R; 
     }
     option(redSB);
     ES=std(ES);
     ES;

     qring q = ES;
     poly F = imap(Px,F);

     isEquising(F);

     kill L,Px;
     kill(q);
     if (defined(R)){ kill(R); }
     kill(r);
   }
}

list L = "ring r = 0,(x,y),ds", "ring r = (0,a),(x,y),ds; minpoly = a^2-1;",
         "ring r = (0, t(1..4)),(x,y),ds", "ring r = 7,(x,y),ds",
         "ring r = (11,a(1)),(x,y),ds; minpoly = a(1)^2-1;",
         "ring r = (3,t(1..8)),(x,y),ds";
tst(L);
kill L;
//////////////////////////////////////////////////////////////////////////////
//  es-deformation of irreducible polynomial x13+x3-3x2y+3xy2

ring r = 0,(A,B,C,D,x,y),dp;
poly F =D3x12+3CD2x11+3C2Dx10+3BD2x10+x13+C3x9+6BCDx9+3AD2x9+3BC2x8+3B2Dx8
 +6ACDx8+3D2x9-3D2x8y+3B2Cx7+3AC2x7+6ABDx7+6CDx8-6CDx7y+B3x6+6ABCx6+3A2Dx6
 +3C2x7+6BDx7-3C2x6y-6BDx6y+3AB2x5+3A2Cx5+6BCx6+6ADx6-6BCx5y-6ADx5y+3A2Bx4
 +3B2x5+6ACx5+3Dx6-3B2x4y-6ACx4y-6Dx5y+3Dx4y2+A3x3+6ABx4+3Cx5-6ABx3y-6Cx4y
 +3Cx3y2+3A2x3+3Bx4-3A2x2y-6Bx3y+3Bx2y2+3Ax3-6Ax2y+3Axy2+x3-3x2y+3xy2-y3;
esStratum(F);
isEquising(F);
kill r;
///////////////////////////////////////////////////////////////////////////
//   es-deformation of irreducible poly in finite characteristic
ring rr = 11,(A,B,C,a,b,x,y),(dp(3),dp(2),dp);
poly F = (y-ax-b*x^2)^5-x^12;
ideal id = (1-A)*a-1,(1+B+A^2)*b-1;
F = F*(1-A)^5*(1+B+A^2)^5;
F = reduce(F,std(id));
esStratum(F);
isEquising(F);
kill(rr);
///////////////////////////////////////////////////////////////////////////
//      1st order es-deformation of irred. poly   
ring rr=0,(A,B,C,x,y),ls;
poly f=x7+y7+(x-y)^2*x2y2;
poly F=f+A*y*diff(f,x)+B*x*diff(f,x)+C*diff(f,y);
list M=esStratum(F,2);
M;
M=esStratum(F,6);
std(M[1][1]);
kill(rr);
/////////////////////////////////////////////////

example esStratum;
example isEquising;
example esIdeal;
example tau_es;

/////////////////////////////////////////////////
//  Andere Namen fuer Variablen + HNE gegeben
ring r=0,(w,v),ds;
poly f=w2-v199;   // simple singularity
list L=hnexpansion(f);
list LL=versal(f);
def Px=LL[1];
setring Px;
list L=imap(r,L);
poly F=Fs[1,1]-A(198)+A(198)*w2;
list M=esStratum(F,2,L);
reduce(F,std(M[1][1]));
M=esStratum(F,L);
reduce(F,std(M[1][1]));
kill LL,Px,r;
/////////////////////////////////////////////////



tst_status(1);$

