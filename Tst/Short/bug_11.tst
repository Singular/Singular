LIB "tst.lib";
tst_init();

ring r;
list l=x,0,y,number(0),z;
ideal(l[1..5]);
kill r;

tst_status(1);$
