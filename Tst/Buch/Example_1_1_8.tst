
LIB "tst.lib";
tst_init();

ring A = 0,x,lp;
number n = 12345/6789;
n^5; 

ring A1 = 32003,x,lp; 
number(123456789)^5;

ring A2 = (2^3,a),x,lp;
number n = a+a2;
n^5;
minpoly;

ring A3 = (2,a),x,lp;
minpoly = a20+a3+1;
number n = a+a2;
n^5;

ring tst = 2,a,lp;
factorize(a20+a2+1,1);
factorize(a20+a3+1,1);

ring R1 = (real,30),x,lp;
number n = 123456789.0;
n^5;

ring R2 = (complex,30,I),x,lp;
number n = 123456789.0+0.0001*I;
n^5;

ring R3 = (0,a,b,c),x,lp;
number n = 12345a+12345/(78bc);
n^2;
n/9c;


tst_status(1);$
