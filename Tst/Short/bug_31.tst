LIB "tst.lib";
tst_init();

ring R2=(0,a),(x,y,z),lp;
minpoly=a3+a+1;
poly f=x4y+xy4;
factorize(f);
//
ring R3=(2,a),(x,y),lp;
poly g=x2+xy+y2;
minpoly=a2+a+1;
factorize(g);
g=g+y3;
factorize(g);

//
ring R4=2,(x,y,z),dp;
ideal I=x2-z,y2-z;
LIB"primdec.lib";
factorize(x2-z);
primdecSY(I);
//
tst_status(1);$
