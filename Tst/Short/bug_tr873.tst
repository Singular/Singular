LIB "tst.lib";
tst_init();

ring Q1=0,i,lp;
qring Q2=i;
for(int i=1;i<5;i++)
{
ring F = 0,(a,b),dp;
ring R = F,(c),dp;
ideal I = c;
std(I);
ring RR = Q2,(c),dp;
ideal I = c;
std(I);

}

tst_status(1);$
