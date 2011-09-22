LIB "tst.lib"; tst_init();
ring r=181,(x,y,z),(c,ls);
ideal id1=maxideal(3);
ideal id2=x2+xyz,y2-z3y,z3+y5xz;
ideal id6=quotient(id1,id2);
id6;
quotient(id2,id1);
module m=x*freemodule(3),y*freemodule(2);
ideal id3=x,y;
quotient(m,id3);
tst_status(1);$
