LIB "tst.lib"; tst_init();
LIB "primdecint.lib";
ring R=integer,(a,b,c,d),dp;
ideal I1=9,a,b;
ideal I2=3,c;
ideal I3=11,2a,7b;
ideal I4=13a2,17b4;
ideal I5=9c5,6d5;
ideal I6=17,a15,b15,c15,d15;
ideal I=intersectZ(I1,I2); I;
I=intersectZ(I,I3); I;
I=intersectZ(I,I4); I;
I=intersectZ(I,I5); I;
I=intersectZ(I,I6); I;
tst_status(1);$
