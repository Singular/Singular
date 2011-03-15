//
// test script for dim over rings
//
LIB "tst.lib";
tst_init();

// case over a field -- just to make sure nothing was broken by adding Z[x]
ring r=0,(x,y),dp;
ideal I=2,x;
dim(std(I));
I=x;
dim(std(I));
kill r;
ring r=0,(x,y),ds;
ideal I=2-x,y;
dim(std(I));
// case over integers 
ring r=integer,(x,y),dp;
ideal I=2,x;
dim(std(I));
I=x;
dim(std(I));
kill r;
// case over Z/2^n
ring r=(integer,2,4),(x,y),dp;
ideal I=2,x;
dim(std(I));
I=3,x;
dim(std(I));
I=x;
dim(std(I));
kill r;
// case over Z/m
ring r=(integer,10),(x,y),dp;
ideal I=2,x;
dim(std(I));
I=3,x;
dim(std(I));
I=x;
dim(std(I));
kill r;

tst_status(1);$
