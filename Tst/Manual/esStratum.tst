LIB "tst.lib"; tst_init();
LIB "equising.lib";
int p=printlevel;
printlevel=1;
ring r = 0,(a,b,c,d,e,f,g,x,y),ds;
poly F = (x2+2xy+y2+x5)+ax+by+cx2+dxy+ey2+fx3+gx4;
list M = esStratum(F);
M[1][1];
printlevel=3;     // displays additional information
esStratum(F,2)  ; // ES-stratum over Q[a,b,c,d,e,f,g] / <a,b,c,d,e,f,g>^2
ideal I = f-fa,e+b;
qring q = std(I);
poly F = imap(r,F);
esStratum(F);
printlevel=p;
tst_status(1);$
