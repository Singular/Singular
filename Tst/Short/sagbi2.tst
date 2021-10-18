LIB "tst.lib";
tst_init();

LIB "sagbigrob.lib";
ring r=ZZ,(x,y,z),dp;
ideal A=2x2+xz,2y2,3yz;
ideal I=4x2y2+2xy2z,6x2yz+3xyz2,4x4-4y4+4x3z+x2z2,27y3z3;
LTGS(I,A);
SPOLY(I,A);
SGB(I,A);

ring r2=0,(x,y),lp;
ideal A=x3y2+y2, x3-y, y2+y;
ideal I=x3y2+x3y-y3-y2, x3+y2, x3y2-y;
LTGS(I,A);
SPOLY(I,A);
SGB(I,A);

example SGB;
example SPOLY;
example LTGS;
example SGNF;

tst_status(1);$
