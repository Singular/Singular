LIB "tst.lib"; tst_init();
LIB "decodegb.lib";
intvec vopt = option(get);
list l=FLpreprocess(3,1,11,2,"");
def r=l[1];
setring r;
int s_work=l[2];
//the check matrix of [11,6,5] ternary code
matrix h[5][11]=1,0,0,0,0,1,1,1,-1,-1,0,
0,1,0,0,0,1,1,-1,1,0,-1,
0,0,1,0,0,1,-1,1,0,1,-1,
0,0,0,1,0,1,-1,0,1,-1,1,
0,0,0,0,1,1,0,-1,-1,1,1;
matrix g=dual_code(h);
matrix x[1][6];
matrix y[1][11]=encode(x,g);
//disturb with 2 errors
matrix rec[1][11]=errorInsert(y,list(2,4),list(1,-1));
//the Fitzgerald-Lax system
ideal sys=sysFL(h,rec,2,1,s_work);
print(sys);
option(redSB);
ideal red_sys=std(sys);
red_sys;
// read the solutions from this redGB
// the points are (0,0,1) and (0,1,0) with error values 1 and -1 resp.
// use list points to find error positions;
points;
option(set,vopt);
tst_status(1);$
