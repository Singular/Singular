LIB "tst.lib"; tst_init();
LIB "ainvar.lib";
//Winkelmann: free action but Spec(k[x(1),...,x(5)]) --> Spec(invariant ring)
//is not surjective
ring rw=0,(x(1..5)),dp;
matrix m[5][1];
m[3,1]=x(1);
m[4,1]=x(2);
m[5,1]=1+x(1)*x(4)+x(2)*x(3);
ideal in=invariantRing(m,x(3),x(1),0);      //compute full invarint ring
in;
//Deveney/Finston: The ring of invariants is not finitely generated
ring rf=0,(x(1..7)),dp;
matrix m[7][1];
m[4,1]=x(1)^3;
m[5,1]=x(2)^3;
m[6,1]=x(3)^3;
m[7,1]=(x(1)*x(2)*x(3))^2;
ideal in=invariantRing(m,x(4),x(1),6);      //all invariants up to degree 6
in;
tst_status(1);$
