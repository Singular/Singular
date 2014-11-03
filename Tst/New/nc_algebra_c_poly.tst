LIB "tst.lib"; tst_init();

proc testWrongNCAlgebra(cc)
{
// def save  = basering;
// type(cc);

 def A=nc_algebra(cc,0);  // must be an error

 if(!defined(A)) { return(); } // OK

// setring A; print(ringlist(A)[5]); setring save;
 
 ERROR("Wrong NC-algebra definition with " + string(typeof(cc)) + "{"+string(cc)+"}" );
}

ring r=0,(x,y),dp;
testWrongNCAlgebra(x2);

matrix c[1][1] = x2;
testWrongNCAlgebra(c);

matrix C[2][2] = 0, x2, 0, 0;
testWrongNCAlgebra(C);



tst_status(1);$
