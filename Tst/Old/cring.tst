int i;
ring r;
"allocating a lot of memory, that is never freed";
for (i=1; i<20; i=i+1)
{
  poly rp(i) = x^i;
}
ring rr;
for (i=1; i<100; i=i+1)
{
  poly rrp(i) = y^i;
}
setring r;
rp(10);
setring rr;
rrp(10);
kill r;
ring r;
for (i=1; i<100; i=i+1)
{
  poly rp(i) = y^i;
}
listvar(all);
LIB "tst.lib";tst_status(1);$
