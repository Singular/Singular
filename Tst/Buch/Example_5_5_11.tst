LIB "tst.lib";
tst_init();

ring A=0,(x,y,z),ds;
ideal I=yz+z2+x3,y2+xz+y4;
ideal J=std(I);
J;

int k;
ideal In=jet(J[1],ord(J[1]));
for(k=2;k<=size(J);k++){ In=In,jet(J[k],ord(J[k]));}

In;

bigintvec v=hilb(J,1);
v;

ideal I1=I+maxideal(5);
vdim(std(I1));

vdim(std(I+maxideal(6)));

tst_status(1);$
