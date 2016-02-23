LIB "tst.lib";
tst_init();

// write/read rings over tansz./alg.extensions
ring R = (0,a),x,dp;
minpoly = a2+1;
link l = "ssi:w ssi_rings";
write(l, R);
write(l,a+2+3x);
ring R2=(7,a),y,dp;
write(l,R2);
write(l,4a+5+6y);
close(l);
l="ssi:r ssi_rings";
def r1=read(l); r1;
def r2=read(l); r2;
def r3=read(l); r3;
l;
def r4=read(l); r4;
l;
def r5=read(l); r5;
close(l);

tst_status(1);$

