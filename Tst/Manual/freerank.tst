LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r;
ideal i=x;
module M=[x,0,1],[-x,0,-1];
freerank(M);          // should be 2, coker(M) is not free
freerank(syz (M),"");
// [1] should be 1, coker(syz(M))=M is free of rank 1
// [2] should be gen(2)+gen(1) (minimal relation of M)
freerank(i);
freerank(syz(i));     // should be 1, coker(syz(i))=i is free of rank 1
tst_status(1);$
