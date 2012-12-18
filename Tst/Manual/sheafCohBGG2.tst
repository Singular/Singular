LIB "tst.lib"; tst_init(); tst_ignore("Time:");
LIB "sheafcoh.lib";
int pl = printlevel;
int l,h;
//-------------------------------------------
// cohomology of structure sheaf on P^4:
//-------------------------------------------
ring r=32001,x(1..5),dp;
module M= getStructureSheaf(); // OO_P^4
l = -12; h = 12; // range of twists: l..h
printlevel = 0;
//////////////////////////////////////////////
timer = 0;
def A = sheafCoh(M, l, h); // global Ext method:
tst_ignore("Time: " + string(timer));
//////////////////////////////////////////////
timer = 0;
A = sheafCohBGG(M, l, h);  // BGG method (without optimization):
tst_ignore("Time: " + string(timer));
//////////////////////////////////////////////
timer = 0;
A = sheafCohBGG2(M, l, h); // BGG method (with optimization)
tst_ignore("Time: " + string(timer));
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
timer = 0;
def B = sheafCoh(M, l, h); // global Ext method:
tst_ignore("Time: " + string(timer));
//////////////////////////////////////////////
timer = 0;
B = sheafCohBGG(M, l, h);  // BGG method (without optimization):
tst_ignore("Time: " + string(timer));
//////////////////////////////////////////////
timer = 0;
B = sheafCohBGG2(M, l, h); // BGG method (with optimization)
tst_ignore("Time: " + string(timer));
//////////////////////////////////////////////
printlevel = pl;
tst_status(1);$
