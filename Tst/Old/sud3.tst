LIB "deform.lib";
option(prot);
ring r4=32003,(w(1..9),x,y,z),(dp);
ideal i=
w(1)-z,w(2)-xz,w(3)-x2z,w(4)-yz,w(5)-xyz,w(6)-x2yz,w(7)-y2z,w(8)-xy2z,w(9)-y3z;
ideal j;
j=eliminate(i,xyz);
ring rr=32003,(w(1..9)),dp;
ideal j=imap(r4,j);
//a fat point!!   T1=1,T2=6;  A^2=0 ";
int time=timer;
miniversal(j);   
setring Ont; 
jetF;jetJ;
"time";timer-time;
LIB "tst.lib";tst_status(1);$ 
