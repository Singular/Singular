LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=0,(x,y,z),dp;
matrix F[1][3]=x+y+z,xy+xz+yz,xyz;
ideal I=xy;
print(image_of_variety(I,F));
tst_status(1);$
