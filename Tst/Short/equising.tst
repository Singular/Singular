LIB "tst.lib";
tst_init();

LIB "sing.lib";
LIB "deform.lib";
LIB "equising.lib";

///////////////////////////////////////////////////////////////////////////////////////////////

proc tst(list d)
{
 int i;
 for(i=1;i<=size(d);i++)
   {
     execute(d[i]);
     
     poly f = y8+4x3y6+6x6y4+2x5y5+4x9y2+4x8y3+x12+2x11y+2x10y2+x13;
               	    
     versal(f);
     setring Px;
     poly F = Fs[1,1];
              
     list EsList = esStratum(F);
     EsList[2];  
     if (typeof(EsList[1])=="ideal") 
     { 
       ideal ES = EsList[1];
     }
     else
     {
       def R=EsList[1]; setring R; 
     }
     option(redSB);
     ES=std(ES);
     ES;

     qring q = ES;
     poly F = imap(Px,F);

     isEquising(F);

     kill(Px);
     kill(Qx);
     kill(Ox);
     kill(So);
     kill(q);
     if (defined(R)){ kill(R); }
     kill(r);
   }
}


list l = 
 "ring r = 0,(x,y),ds",
 "ring r = (0,a),(x,y),ds; minpoly = a^2-1;",
 "ring r = (0, t(1..4)),(x,y),ds",
 "ring r = 7,(x,y),ds",
 "ring r = (11,a(1)),(x,y),ds; minpoly = a(1)^2-1;",
 "ring r = (3,t(1..8)),(x,y),ds"
;


tst(l);

//////////////////////////////////////////////////////

 ring r = 0,(A,B,C,D,x,y),dp;
 poly F =D3x12+3CD2x11+3C2Dx10+3BD2x10+x13+C3x9+6BCDx9+3AD2x9+3BC2x8+3B2Dx8+6ACDx8+3D2x9-3D2x8y+3B2Cx7+3AC2x7+6ABDx7+6CDx8-6CDx7y+B3x6+6ABCx6+3A2Dx6+3C2x7+6BDx7-3C2x6y-6BDx6y+3AB2x5+3A2Cx5+6BCx6+6ADx6-6BCx5y-6ADx5y+3A2Bx4+3B2x5+6ACx5+3Dx6-3B2x4y-6ACx4y-6Dx5y+3Dx4y2+A3x3+6ABx4+3Cx5-6ABx3y-6Cx4y+3Cx3y2+3A2x3+3Bx4-3A2x2y-6Bx3y+3Bx2y2+3Ax3-6Ax2y+3Axy2+x3-3x2y+3xy2-y3;

 esStratum(F);

 isEquising(F);


/////////////////////////////////////////////////
ring rr = 11,(A,B,C,a,b,x,y),(dp(3),dp(2),dp);

poly F = (y-ax-b*x^2)^5-x^12;
ideal id = (1-A)*a-1,(1+B+A^2)*b-1;
F = F*(1-A)^5*(1+B+A^2)^5;
F = reduce(F,std(id));

esStratum(F);

isEquising(F);

kill(rr);
/////////////////////////////////////////////////

ring rr=0,(A,B,C,x,y),ls;
poly f=x7+y7+(x-y)^2*x2y2;
poly F=f+A*y*diff(f,x)+B*x*diff(f,x)+C*diff(f,y);
list M=esStratum(F,6);
std(M[1]);

kill(rr);
/////////////////////////////////////////////////

example esStratum;
example isEquising;
example esIdeal;
example tau_es;

/////////////////////////////////////////////////

ring rr=0,(x,y),ds;
poly f=x4+4x3y+6x2y2+4xy3+y4+2x2y15+4xy16+2y17+xy23+y24+y30+y31;
list K=esIdeal(f);
K;
vdim(std(K[1]));  
tau_es(f);    

/////////////////////////////////////////////////



tst_status(1);$

