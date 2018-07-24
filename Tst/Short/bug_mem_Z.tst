LIB "tst.lib";
tst_init();

// memory leak in std in Z[x,y]:
int i;
bigint m=memory(0);
ring r=ZZ,(x,y),(dp,c);
ideal I=x+y,x-y;
ideal J;
for (i=0;i<10;i++)
{
 J=std(I);
}
kill r;
memory(0)-m;
ring r=ZZ,(x,y),(dp,c);
ideal I=x+y,x-y;
ideal J;
for (i=0;i<10;i++)
{
 J=std(I);
}
kill r;
memory(0)-m;

tst_status(1);$
