LIB "tst.lib";
tst_init();

// an error  about missing monomials after chinrem:
ring r=0,x,dp;
ideal i1=2x+3x2+5x4;
ideal i2=1x+6x2+51x4;
ideal i3=1x+6x2+7x4;
list l=i1,i2,i3;
intvec v=181,32003,31991;
chinrem(l,v);
i2=1x+51x4;
l=i1,i2,i3;
chinrem(l,v);
i2=51x4;
l=i1,i2,i3;
chinrem(l,v);
i2=x;
l=i1,i2,i3;
chinrem(l,v);
l=ideal(x2),ideal(x),ideal(x3);
chinrem(l,v);

tst_status(1);$
