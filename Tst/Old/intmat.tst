//
// intmat.tst
//

intvec iv = 1,3,5;
intmat m[4][3];
m=iv,1,2,3,4,5,6,iv;
m;
m*iv;
intmat n[4][3];
n=1,2,3,4,5,7,iv,iv;
n*transpose(m);
n*m;
n+m;
n-m;
trace(m*transpose(n));
m[1,2..3];
m[3..4,2];
intmat sub[2][2];
intvec i1=2,4;
intvec i2=1,3;
sub=n[i1,i2];
sub;
kill iv,n,m,i1,i2,sub;

intmat i[2][3];
i=1,2,3,4,5,0,1;
intmat j;
listvar(all);
size(i);
nrows(i);
ncols(i);
i;
j;
i[2,1];
i[0,3];
i[20];
j=i;
j;
listvar(all);
i + 2;
i - 2;
i * 2;
i / 2;
i == 2;
intvec t=i;
intmat t=i;
i==t;
i + i;
i*j;
transpose(j)*j;
trace(transpose(j)*j);
LIB "tst.lib";tst_status(1);$
