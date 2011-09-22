LIB "tst.lib"; tst_init();
    ring r = (real,10),(x),lp;

    // consider the max. problem:
    //
    //    maximize  x(1) + x(2) + 3*x(3) - 0.5*x(4)
    //
    //  with constraints:   x(1) +          2*x(3)          <= 740
    //                             2*x(2)          - 7*x(4) <=   0
    //                               x(2) -   x(3) + 2*x(4) >=   0.5
    //                      x(1) +   x(2) +   x(3) +   x(4)  =   9
    //
    matrix sm[5][5]=(  0, 1, 1, 3,-0.5,
                     740,-1, 0,-2, 0,
                       0, 0,-2, 0, 7,
                     0.5, 0,-1, 1,-2,
                       9,-1,-1,-1,-1);

    int n = 4;  // number of constraints
    int m = 4;  // number of variables
    int m1= 2;  // number of <= constraints
    int m2= 1;  // number of >= constraints
    int m3= 1;  // number of == constraints
    simplex(sm, n, m, m1, m2, m3);
tst_status(1);$
