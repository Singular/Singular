LIB "tst.lib";
tst_init();

ring R=0,(x,y,z),ls;
poly p= xyz+x2yz+xy2z;
ideal m=x+random(-5,5)*z,y+random(-5,5)*z,z;
map phi=R,m;
phi(p);

tst_status(1);$
