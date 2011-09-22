LIB "tst.lib"; tst_init();
LIB "reszeta.lib";
ring R=0,(x,y,z),dp;
ideal I=xyz+x4+y4+z4;
//we really need to blow up curves even if the generic point of
//the curve the total transform is n.c.
//this occurs here in r[2][5]
list re=resolve(I);
list di=collectDiv(re);
di[1];
di[2];
tst_status(1);$
