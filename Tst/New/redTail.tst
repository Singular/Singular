LIB "tst.lib";
tst_init();

ring R=0,x(0..4),dp;
ideal I=-x(3)^2+x(2)*x(4),-x(2)*x(3)+x(1)*x(4),-x(1)*x(3)+x(0)*x(4),x(2)^2-x(1)*x(3),x(1)*x(2)-x(0)*x(3),-x(1)^2+x(0)*x(2);
I;
// option (teach);
option (prot); option (warn); test (23+32); option();

std(I);

option();

tst_status(1);
$
