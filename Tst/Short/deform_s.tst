//deform_s.tst
//short tests for deform.lib
//---------------------------
LIB "tst.lib";
tst_init();
LIB "deform.lib";
example versal;
example mod_versal;
example lift_rel_kb;
example lift_kbase;
   printlevel =  2; 
ring  r1  = 0,(x,y,z,u),dp;
 ideal i3 = xy,xz,xu,yz,yu,zu;       
 versal(i3);
 setring Px;
 listvar(Px);
 kill_rings();
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
  mod_versal(Mo,Io);
  setring Px;
  listvar(Px);
  setring(Po);
 kill_rings();
}
$
