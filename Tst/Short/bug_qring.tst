LIB "tst.lib";
tst_init();

// normal form and complete NF in qrings:

ring p=0,(y,z,u,v),dp;
option(redSB);
qring q=std(ideal(y+u2+uv3,z+uv3));
q;
ideal i=y,z;
std(i);
NF(std(i),0);
interred(std(i));
setring p;
ideal i=y,z,y+u2+uv3,z+uv3;
option(redSB);
std(i);                    
NF(_,std(ideal(y+u2+uv3,z+uv3)));
interred(std(i));
interred(NF(std(i),std(ideal(y+u2+uv3,z+uv3))));

ring r = 2,(a, b),dp;
option(redSB);
interred(ideal(a^2,a*b+a^2));
qring qq = std(ideal(b^2+a*b+a^2,a^3));
qq;
option(redSB);
interred(ideal(a^2,a*b+a^2));

tst_status(1);$
