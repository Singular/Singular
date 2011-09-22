LIB "tst.lib"; tst_init();
LIB "presolve.lib";
// Solve the system of linear equations:
//         3x +   y +  z -  u = 2
//         3x +  8y + 6z - 7u = 1
//        14x + 10y + 6z - 7u = 0
//         7x +  4y + 3z - 3u = 3
ring r = 0,(x,y,z,u),lp;
ideal i= 3x +   y +  z -  u,
13x +  8y + 6z - 7u,
14x + 10y + 6z - 7u,
7x +  4y + 3z - 3u;
ideal j= 2,1,0,3;
j = matrix(i)-matrix(j);        // difference of 1x4 matrices
// compute reduced triangular form, setting
solvelinearpart(j);             // the RHS equal 0 gives the solutions!
solvelinearpart(j,1); "";       // triangular form, not reduced
tst_status(1);$
