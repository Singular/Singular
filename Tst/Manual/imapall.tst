LIB "tst.lib"; tst_init();
LIB "ring.lib";
// The example is not shown since imapall does not work in a procedure
// (and hence not in the example procedure). Try the following commands:
//   ring R=0,(x,y,z,u),dp;
//   ideal j=x,y,z,u2+ux+z;
//   matrix M[2][3]=1,2,3,x,y,uz;
//   j; print(M);
//   ring S=0,(a,b,c,x,z,y),ds;
//   imapall(R);         //map from R to S: x->x, y->y, z->z, u->0
//   names(S);
//   j; print(M);
//   imapall(S,"1");     //identity map of S: copy objects, change names
//   names(S);
//   kill R,S;
tst_status(1);$
