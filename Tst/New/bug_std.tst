LIB "tst.lib";
tst_init();

ring q = (integer,2,3),(r,s,a,b,b_2,b_1,b_0),lp;
poly p0 = b -1*b_0 -2*b_1 -4*b_2;
poly p1 = r-a*b_0;
poly p2 = s*b_0;
poly p3 = s*b_1;
poly p4 = s*b_2;
ideal I = (p0,p1,p2,p3,p4);
poly goal = r- a*b;
poly p = 1 - s;
I = std(I);
print("-------std(I+p)-------");
std(I+p);
print("-------std(I,p)-------");
std(I,p);

tst_status(1);
$
