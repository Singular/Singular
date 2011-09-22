LIB "tst.lib"; tst_init();
LIB "mregular.lib";
ring s=0,(x,y,z,t),dp;
ideal i1=x2,y3; ideal i2=x3,y2,z2; ideal i3=x3,y2,t2;
ideal i=intersect(i1,i2,i3);
is_nested(i);
ideal ch=x,y,z,z+t;
map phi=ch;
ideal I=lead(std(phi(i)));
is_nested(I);
tst_status(1);$
