//
// intvec.tst
//

intvec iv = 1,3,5;
iv;
iv[1];
iv[1] + iv[2] + iv[3];
iv[2] = 42;
iv;
intvec ivv = 4, iv;
ivv;
iv[iv[1]+1] + ivv[2];
iv+ivv;
iv-ivv;
intmat m=transpose(ivv);
m;
intmat n=m+ivv;
kill iv,ivv,m;

intvec i;
listvar(all);
intvec j=1,2,3,4,5,0;
listvar(all);
i;
j;
j[2];
j[0];
j[20];
i=j;
i;
listvar(all);
i=1..10;
i;
i=2,3,6;
j[i];

intvec i = 1 .. 5;
i;
size(i);
ncols(i);
nrows(i);
i + 2;
i - 2;
i * 2;
i / 2;
i == 2;
intvec t=i;
i==t;
i;
i + i;
i;
i*j;
transpose(j)*j;
trace(j);
LIB "tst.lib";tst_status(1);$
