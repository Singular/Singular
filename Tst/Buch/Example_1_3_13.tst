LIB "tst.lib";
tst_init();

ring R = 32003,(x,y,z),dp;
ideal I = x2+y2-z5, z-x-y2;     
qring Q = groebner(I);  
Q;

poly f = z2 + y2;
poly g = z2+2x-2z-3z5+3x2+6y2;
reduce(f-g,std(0)); 

setring R;
poly f = z2 + y2;
poly g = z2 + 2x - 2z - 3z5 + 3x2 + 6y2;
reduce(f-g,groebner(I));

setring Q;
ideal q = quotient(0,f);
q = reduce(q,std(0));     
size(q);              
kill R;
kill Q;

ring R=(0,x),(y,z),dp;
ideal I=-z5+y2+(x2),-y2+z+(-x);
I=std(I);qring Q=I;
poly p=lift(z,1)[1,1];
p;
reduce(p*z,std(0));

ring R1=(0,x),(z,y),lp;
ideal I=imap(R,I);
I=std(I);
I;

factorize(I[1]);

tst_status(1);$
