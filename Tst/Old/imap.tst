ring r1=0,(x,y,a,b,z,c),dp;
poly f=x+2y+3z+4a+5b+6c+11x2+12xy+13xz+14xa+15xb+16xc+25y2b3;
f;
ring r2=0,(x,z,y),dp;
imap(r1,f);
poly f=imap(r1,f);
f;
listvar(all);
LIB "tst.lib";tst_status(1);$
