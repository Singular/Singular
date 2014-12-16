LIB "tst.lib";
tst_init();

option();

ring R=0,x(0..4),dp;
ideal I=-x(3)^2+x(2)*x(4),-x(2)*x(3)+x(1)*x(4),-x(1)*x(3)+x(0)*x(4),x(2)^2-x(1)*x(3),x(1)*x(2)-x(0)*x(3),-x(1)^2+x(0)*x(2);
I;
// option (teach);
option (prot);
option (warn); // test (23+32);
option();

std(I);

proc T()
{
 option();
 ring r;
 option();
 def I = std(maxideal(1));
 option ();
 ring rr = 0,(x, y, z), ds;
 option();
 def I = std(maxideal(1));
 option ();

}

option();
T();
option();

tst_status(1);
$
