LIB "tst.lib";
tst_init();

ring A  =0,(x,y,z),dp;
ideal I1=x5z3,xyz,yz4;
ideal I2=z;

LIB"elim.lib";
sat(I1,I2);

int k;
ideal J=quotient(I1,I2);
while(size(reduce(J,std(I1)))!=0)
{
   k++;
   I1=J;
   J=quotient(J,I2);
}
J;
k;


tst_status(1);$
