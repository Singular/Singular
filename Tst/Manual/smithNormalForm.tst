LIB "tst.lib"; tst_init();
LIB "multigrading.lib";
intmat A[5][7] =
1,0,1,0,-2,9,-71,
0,-24,248,-32,-96,448,-3496,
0,4,-42,4,-8,30,-260,
0,0,0,18,-90,408,-3168,
0,0,0,-32,224,-1008,7872;
print( smithNormalForm(A) );
list l = smithNormalForm(A, 5);
l;
l[1]*A*l[3];
det(l[1]);
det(l[3]);
tst_status(1);$
