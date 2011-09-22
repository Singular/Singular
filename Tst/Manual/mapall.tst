LIB "tst.lib"; tst_init();
LIB "ring.lib";
// The example is not shown since mapall does not work in a procedure
// (and hence not in the example procedure). Try the following commands:
//   ring R=0,(x,y,z),dp;
//   ideal j=x,y,z;
//   matrix M[2][3]=1,2,3,x,y,z;
//   map phi=R,x2,y2,z2;
//   ring S=0,(a,b,c),ds;
//   ideal i=c,a,b;
//   mapall(R,i);         //map from R to S: x->c, y->a, z->b
//   names(S);
//   j; print(M); phi;    //phi maps R to S: x->c2, y->a2, z->b2
//   ideal i1=a2,a+b,1;
//   mapall(R,i1,"");     //map from R to S: x->a2, y->a+b, z->1
//   names(S);
//   j_; print(M_); phi_;
//   changevar("T","x()",R);  //change vars in R and call result T
//   mapall(R,maxideal(1));   //identity map from R to T
//   names(T);
//   j; print(M); phi;
//   kill R,S,T;
tst_status(1);$
