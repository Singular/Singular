LIB "tst.lib";
tst_init();

ring r=0,(x,y,z),dp;
ideal i=x*y*z-1,x+y+z,x*y+x*z+y*z; // cyclic 3
janet(i);

ring r=0,(x(1..5)),dp;
LIB "polylib.lib";
ideal i=cyclic(5);
ideal j=janet(i);
ideal s=std(j);
j;
NF(j,s);
NF(s,std(j));

tst_status(1);$
