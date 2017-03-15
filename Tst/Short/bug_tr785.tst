LIB "tst.lib";
tst_init();

option(prot);
ring sds = integer,(t,x1,x2,x3,x4),ds;
ideal I = 3-t,2*x1^2+3*x1*x2+24*x3*x4,8*x1^3+x2*x3*x4+18*x3^2*x4;
I=std(I);
I;
ord(I[1..ncols(I)]);

ring sws = integer,(t,x1,x2,x3,x4),ws(1,1,1,1,1);
ideal I = 3-t,2*x1^2+3*x1*x2+24*x3*x4,8*x1^3+x2*x3*x4+18*x3^2*x4;
I=std(I);
I;
ord(I[1..ncols(I)]);

ring salp = integer,(t,x1,x2,x3,x4),(a(-1,-1,-1,-1,-1),lp);
ideal I = 3-t,2*x1^2+3*x1*x2+24*x3*x4,8*x1^3+x2*x3*x4+18*x3^2*x4;
I=std(I);
I;
ord(I[1..ncols(I)]);

tst_status(1);$
