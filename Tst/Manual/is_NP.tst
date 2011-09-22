LIB "tst.lib"; tst_init();
LIB "mregular.lib";
ring r=0,(x,y,z,t,u),dp;
ideal i1=y,z,t,u; ideal i2=x,z,t,u; ideal i3=x,y,t,u; ideal i4=x,y,z,u;
ideal i5=x,y,z,t; ideal i=intersect(i1,i2,i3,i4,i5);
is_NP(i);
ideal ch=x,y,z,t,x+y+z+t+u;
map phi=ch;
is_NP(phi(i));
tst_status(1);$
