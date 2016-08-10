LIB "tst.lib"; tst_init();

ring R=0,(x,y),(a(1,-1),lp);
ideal P=x2y+1; // P is a prime ideal of dim 0 in R
P;
dim(P); // expect a warning
dim(P,ideal(0));

tst_status(1);$

