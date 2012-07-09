LIB "tst.lib";
tst_init();
// an error about choosing field extensions

ring r=2,(v1,v2,v3,v4,v5),dp;
ideal I=v3^2+v1*v3+v1, v4^2+v2*v4+v2,
v5^2+v1*v2*v5+v1*v2^2+v1^2*v2;
I;
for(int i=0;i<10;i++)
{
print(char_series(I));
"==============================";
}

tst_status(1);$
