LIB "tst.lib"; tst_init();

//   characteristic : 0
//   1 parameter    : t
//   minpoly        : 0
//   number of vars : 2
//        block   1 : ordering ds
//                  : names    x y
//        block   2 : ordering C
ring R = (0,t),(x,y),(ds(2),C); R;

poly F = 2*x2+(-1/2t)*y2-y3; F;

subst(F,t,1/10); // 2*x2-1/10*y2-y3

poly G = (-1/16t5)+(-5/16t4)*y+2*x2+(5/4t2)*y3-y5; G;

subst(G,t,1/10); // -1/100000-1/2000*y+2*x2+1/20*y3-y5

tst_status(1);$
