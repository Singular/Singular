LIB "tst.lib"; tst_init();
LIB "polylib.lib";
ring r=32003,(a,b,c,d,e,f),lp;
ideal I=cyclic(6);
option(prot);
int t=timer;
system("--ticks-per-sec", 100);         // give time in 1/100 sec
ideal sI=std(I);
// timer-t;                                // used time (in 1/100 secs)
size(sI);
t=timer;
sI=groebner(I);
// timer-t;                                // used time (in 1/100 secs)
size(sI);
option(noprot);
tst_status(1);$
