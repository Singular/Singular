LIB "tst.lib"; tst_init();
LIB "ring.lib";
// The example is not shown since fetchall does not work in a procedure;
// (and hence not in the example procedure). Try the following commands:
//   ring R=0,(x,y,z),dp;
//   ideal j=x,y2,z2;
//   matrix M[2][3]=1,2,3,x,y,z;
//   j; print(M);
//   ring S=0,(a,b,c),ds;
//   fetchall(R);       //map from R to S: x->a, y->b, z->c;
//   names(S);
//   j; print(M);
//   fetchall(S,"1");   //identity map of S: copy objects, change names
//   names(S);
//   kill R,S;
tst_status(1);$
