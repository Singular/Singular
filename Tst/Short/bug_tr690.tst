LIB "tst.lib";
tst_init();

// memeory leak: local ordering, ring-cf
ring r=integer,t,ds;
ideal I = 2,3-t;
int i;
bigint m=memory(0);
for(i=1; i<100; i++)
{
  ideal II = std(I);
  kill II;
}
m-memory(0); //  should be 0

tst_status(1);$

