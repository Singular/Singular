LIB "tst.lib";
tst_init();

LIB"homolog.lib";

proc flatLocus1(matrix M)
{
  list l=mres(M,2);
  return(Ann(Ext(1,M,l[2])));
}
ring R=0,(x,y,z),dp;
qring S=std(xyz);
matrix M[3][3]=x,y,z,0,0,0,xyz,x3,z3;
print(M);

flatLocus1(M);

tst_status(1);$
