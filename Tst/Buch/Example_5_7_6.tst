LIB "tst.lib";
tst_init();

ring R=0,(u,v,w,x,y,z),dp;
ideal I=wx,wy,wz,vx,vy,vz,ux,uy,uz,y3-x2;
LIB"primdec.lib";
radical(I);

list l=minAssGTZ(I);
l;

ideal J=l[1]+l[2];
std(J);

ideal sing=l[2]+minor(jacob(l[2]),4);
std(sing);

tst_status(1);$
