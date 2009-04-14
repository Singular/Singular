LIB "tst.lib";
tst_init();

LIB"normal.lib";
ring A  =0,(x,y,z),wp(2,3,6);
ideal I =y;
ideal J =zy2-zx3-x6;
list nor=normalC(J);
def R   =nor[1][1];
setring R;
norid;

normap;


map phi=A,normap;
ideal I=phi(I)+norid;

std(I);

radical(I);
kill A;

LIB"surf.lib";
ring A=0,(t,x,y),dp;
ideal J=t3-6t+x,-t4+6t2+y;
ideal I=eliminate(J,t);
I;

//plot(I);

tst_status(1);$
