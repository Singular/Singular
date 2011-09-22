LIB "tst.lib"; tst_init();
LIB "primitiv.lib";
ring exring=0,(x,y),dp;
ideal i=x2+1,y2-x;                  // compute Q(i,i^(1/2))=:L
ideal j=primitive(i);
j[1];                               // L=Q(a) with a=(-1)^(1/4)
j[2];                               // i=a^2
j[3];                               // i^(1/2)=a
// the 2nd element was already primitive!
j=primitive(ideal(x2-2,y2-3));      // compute Q(sqrt(2),sqrt(3))
j[1];
j[2];
j[3];
// no element was primitive -- the calculation of primitive elements
// is based on a random choice.
tst_status(1);$
