LIB "tst.lib";
tst_init();

ring A=0,(x,y,z),dp;
ideal I=y3-z2,x3-z;
qhweight(I);

homog(I);

ring B=0,(x,y,z),wp(1,2,3);
ideal I=fetch(A,I);
homog(I);

module M=[y3-z2,x3-z],[x3,1];
homog(M);

attrib(M,"isHomog");

intvec v=4,7;
attrib(M,"isHomog",v);
attrib(M,"isHomog");

tst_status(1);$
