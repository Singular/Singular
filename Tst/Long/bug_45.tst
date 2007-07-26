LIB "tst.lib";
tst_init();
// segmentation violation during change of tailRing in completeReduce

option(redSB,prot);
int k = 3;
ring R = 0, (z11, z12, z13, z21, z22, z23, z31, z32, z33), lp;
proc g(int i)
{
  return (var(i)^k-1);
}
proc h(int i, int j)
{
  return ((var(i)^k-var(j)^k)/(var(i)-var(j)));
}
ideal I =
h(1,5),
h(2,4),
h(1,6),
h(3,4),
h(2,6),
h(3,5),
h(1,8),
h(2,7),
h(1,9),
h(3,7),
h(2,9),
h(3,8),
h(4,8),
h(5,7),
h(4,9),
h(6,7),
h(5,9),
h(6,8),
g(1), g(2), g(3), g(4), g(5), g(6), g(7), g(8), g(9);
I;

matrix m= lift(I, h(1,4)*h(1,2));
//m;
matrix(h(1,4)*h(1,2))-matrix(I)*m;
tst_status(1);$
