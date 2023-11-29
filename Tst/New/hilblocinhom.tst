LIB "tst.lib";
tst_init();

option(prot);
ring R=0,(x,y,z),ds;
ideal i= x3yz+x10,xy2z2,xyz3;
ideal si=std(i);
bigintvec v=hilb(si,1);
ideal ss=std(i,v);
for(int l=1;l<=size(si);l++)
{
  if(si[l]!=ss[l])
  {"false";}
}


option(prot);
ring R=0,(x,y,z),ls;
ideal i= x3yz+x100,xy2z2+x13y66,xyz3+z10;
ideal si=std(i);
bigintvec v=hilb(si,1);
ideal ss=std(i,v);
for(int l=1;l<=size(si);l++)
{
  if(si[l]!=ss[l])
  {"false";}
}


tst_status(1);
$
