LIB "tst.lib"; tst_init();
LIB "finvar.lib";
ring R=0,(x,y,z),dp;
matrix A[3][3]=0,1,0,-1,0,0,0,0,-1;
list L=group_reynolds(A);
list l=primary_char0_no_molien_random(L[1],1);
print(l[1]);
tst_status(1);$
