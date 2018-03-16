LIB "tst.lib";
tst_init();

// Ring declarations of the form (ZZ/n)[x] work wrong when n is a power of 2
int i;
ring R1=(ZZ/16)[x]; R1;
number n=2;
for(i=1;i<3;i++) { n=n*n;n; }
number(32768);
number(65536);
ring R2=(ZZ/(bigint(2)^64))[x]; R2;
number n=2;
for(i=1;i<7;i++) { n=n*n;n; }
number(32768);
number(65536);
ring R3=(ZZ/(bigint(2)^65))[x]; R3;
number n=2;
for(i=1;i<8;i++) { n=n*n;n; }
number(32768);
number(65536);

tst_status(1);$
