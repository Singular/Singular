LIB "tst.lib";
tst_init();

ring R=(0,a),(x,y),lp;
minpoly=a3+a+1;
factorize(poly(3a));
factorize(poly(3));
factorize(x);
factorize(y2);
factorize(xy2);
ring R3=(32003,a),(x,y),lp;
minpoly=a3+a+1;
factorize(poly(3a));
factorize(poly(3));
factorize(x);
factorize(y2);
factorize(xy2);
ring R4=32003,(x,y),lp;
factorize(poly(3));
factorize(x);
factorize(y2);
factorize(xy2);
ring R5=0,(x,y),lp;
factorize(poly(3));
factorize(x);
factorize(y2);
factorize(xy2);
ring R2=(0,a),(x,y,z),lp;
minpoly=a3+a+1;
poly f=x4y+xy4;
factorize(xy2);
factorize(f);
ring R22=(0,a),(x,y),lp;
minpoly=a2+1;
factorize (x3+y3);

tst_status(1);$
