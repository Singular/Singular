//deform_s.tst
//short tests for deform.lib
//---------------------------
LIB "tst.lib";
tst_init();
LIB "deform.lib";
example versal;
// rest of what used to be in that example
   int p = printlevel;
   ring  r2       = 0,(x,y,z),ds;
   ideal Fo       = x2,xy,yz,zx;
   printlevel     = 2;
   def L=versal(Fo);
   def Px=L[1];def So=L[3];
   setring(So);
   ideal Js=imap(Px,Js);
   hilb(std(Js)); 
   printlevel     = p;
   kill L,Px,So;
example mod_versal;
example lift_rel_kb;
// rest of what used to be in that example
  "2nd EXAMPLE";
  ring   r = 100,(x,y),dp;
  ideal  I = x2+y2,x2y;
  module M = jacob(I)+I*freemodule(2);
  module N = [x+y,1+x2+xy];
  matrix A = lift_rel_kb(N,M);
  print(A);
  print(kbase(std(M))*A);
  print(reduce(N,std(M)));
example lift_kbase;
   printlevel =  2; 
ring  r1  = 0,(x,y,z,u),dp;
 ideal i3 = xy,xz,xu,yz,yu,zu;       
 list L=versal(i3);
 def Px=L[1]; def So=L[3];
 setring Px;
 listvar(Px);
 size(reduce(Fs*Rs,std(ideal(Js))));
 setring So;
 ideal Js=imap(Px,Js);
 hilb(std(Js)); 
 kill L,Px,So;
ring   Po = 0,(x,y),dp;
ideal  Io = std(x^4+y^3);
matrix Mo;
//============= rk 1 ======================================
module k =[x],[y];
module m(0)=k;
module m(1)=[x,y],[-y2,x3];
module m(2)=[x3,y],[-y2,x];
int i';
for (i'=0;i'<3;i'=i'+1)
{
  Mo=m(i');
  list L=mod_versal(Mo,Io);
  def Px=L[1]; def Qx=L[2]; def So=L[3]; 
  setring Px;
  listvar(Px);
  setring(So);
  ideal Js=imap(Qx,Js);
  hilb(std(Js)); 
  setring(Po);
  kill L,Px,Qx,So;
}
tst_status(1);$
