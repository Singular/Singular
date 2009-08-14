LIB "tst.lib";
tst_init();

// only debug version produces this error:
ring r = 0,(x, y), dp;
intmat A[2][2] = 1, 0, 0, 1;
attrib(basering, "mgrad", A); 

proc getGrad2(){ attrib(basering, "mgrad"); }
getGrad2(); // produces nothing!

proc getGrad(def A){ attrib(A, "mgrad"); }

getGrad(basering); // Produces: "atKill: unknown type(267)"
/*
::getGrad   0. parameter def A;
::getGrad   1>  attrib(A, "mgrad");
1,0,
0,1
::getGrad   2>
atKill: unknown type(267)
*/
tst_status(1);$
