// resolution of a space curve ...
ring r=0,(t,x,y,z,l),(dp,c);
ideal i=z2+lx4z-x5y,y2+l3x2z-xz-2lx5-l2x3y,yz+l2x3z-x6-lx4y;
ideal i2=std(i);
list resii=sres(i2,0);
list resi=minres(resii);
resi[1];
resi[2];
print(betti(resi),"betti");
LIB "tst.lib";tst_status(1);$
