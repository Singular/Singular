LIB "tst.lib";
tst_init();

ring A  =0,(x,y,z,a,b,c),lp;
ideal M =a-(xy)^3-x2-z,b-y2+1,c-z3,xy;
ideal SM=std(M);
lead(SM);
kill A;

ring  B=0,(x,y,z),dp;
ideal I=x-zy;
LIB"surf.lib";
//plot(I);

LIB"algebra.lib";
ring A=0,(u,v),dp;
setring B;
map phi1=A,x,y;
mapIsFinite(phi1,A,I);

map phi2=A,y,z;
mapIsFinite(phi2,A,I);

tst_status(1);$
