LIB "tst.lib"; tst_init();
LIB "ncalg.lib";
// example 3.1.4 from the [SST] without the vector w
intmat A[2][4]=3,2,1,0,0,1,2,3;
print(A);
def D1 = GKZsystem(A,"lp","ect");
setring D1;
D1;
print(GKZid);
// now, consider A with the vector w=1,1,1,1
intvec v=1,1,1,1;
def D2 = GKZsystem(A,"lp","blr",v);
setring D2;
print(GKZid);
tst_status(1);$
