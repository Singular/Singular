LIB "tst.lib"; tst_init();

ring R = 0, (x,y,z), (A(3, 2, 2), lp(3), C);

R;


//   characteristic : 0
//   number of vars : 3
//        block   1 : ordering A
//                  : names    x y z
//                  : weights  3 2 2
//        block   2 : ordering lp
//                  : names    x y z
//        block   3 : ordering C$39 = void

ideal G =
x2+y3,
x2y,
x3,
z4
;

// attrib(G, "isSB", 1);

ideal M =
x2+y3,
z4,
y4,
xy3
;

// TRACE=3; option(prot);
division(G, M); // idLift(Gomega, M, NULL, FALSE, FALSE, TRUE, NULL);

tst_status(1);$
