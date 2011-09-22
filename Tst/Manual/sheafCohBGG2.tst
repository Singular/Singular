LIB "tst.lib"; tst_init();
LIB "sheafcoh.lib";
int pl = printlevel;
int l,h, t;
//-------------------------------------------
// cohomology of structure sheaf on P^4:
//-------------------------------------------
ring r=32001,x(1..5),dp;
module M= getStructureSheaf(); // OO_P^4
l = -12; h = 12; // range of twists: l..h
printlevel = 0;
//////////////////////////////////////////////
t = timer;
def A = sheafCoh(M, l, h); // global Ext method:
"Time: ", timer - t;
//////////////////////////////////////////////
t = timer;
A = sheafCohBGG(M, l, h);  // BGG method (without optimization):
"Time: ", timer - t;
//////////////////////////////////////////////
t = timer;
A = sheafCohBGG2(M, l, h); // BGG method (with optimization)
"Time: ", timer - t;
//////////////////////////////////////////////
printlevel = pl;
kill A, r;
//-------------------------------------------
// cohomology of cotangential bundle on P^3:
//-------------------------------------------
ring R=32001,(x,y,z,u),dp;
module M = getCotangentialBundle();
l = -12; h = 11; // range of twists: l..h
//////////////////////////////////////////////
printlevel = 0;
t = timer;
def B = sheafCoh(M, l, h); // global Ext method:
"Time: ", timer - t;
//////////////////////////////////////////////
t = timer;
B = sheafCohBGG(M, l, h);  // BGG method (without optimization):
"Time: ", timer - t;
//////////////////////////////////////////////
t = timer;
B = sheafCohBGG2(M, l, h); // BGG method (with optimization)
"Time: ", timer - t;
//////////////////////////////////////////////
printlevel = pl;
tst_status(1);$
