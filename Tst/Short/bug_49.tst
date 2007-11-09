LIB "tst.lib";
tst_init();

// normal form in a local ring with HEdge of monomial
// lost the coefficient of it.

ring R=0,(x,y,z),ws(5,4,6);
option(prot);
option(teach);
ideal j=xz,4y3+z2+3y2z,x2+2yz+y3;
ideal s=std(j);
s;
system("HC");
reduce(yz2,s); //1/8z3
reduce(y3z,s); //-1/4z3
reduce(yz2+y3z,s); //-1/8z3

ring R1=0,(x,y,z),ds;
option(prot);
option(teach);
ideal j=xz,4y3+z2+3y2z,x2+2yz+y3;
ideal s=std(j);
s;
reduce(yz2,s);
reduce(y3z,s);
reduce(yz2+y3z,s); // the sum ?


tst_status(1);$
