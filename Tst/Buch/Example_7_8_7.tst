LIB "tst.lib";
tst_init();

proc projdim(module M)
{
   list l=mres(M,0);
   int i;
   while(i<size(l))
   {
     i++;
     if(size(l[i])==0){return(i-1);}
   }   
}

proc isCohenMacaulay1(ideal I)
{
  int de=nvars(basering)-projdim(I*freemodule(1));
  int di=dim(std(I));
  return(de==di);
}
ring R=0,(x,y,z),ds;
ideal I=xz,yz;
isCohenMacaulay1(I);


I=x2-y3;
isCohenMacaulay1(I);

I=xz,yz,xy;
isCohenMacaulay1(I);
kill R;

proc CohenMacaulayTest1(module M, int d)
{
  return((d-projdim(M))==dim(std(Ann(M)))); 
}

LIB"primdec.lib";
ring R=0,(x,y,z),ds;
ideal I=xz,yz;
module M=I*freemodule(1);
CohenMacaulayTest1(M,3);

I=x2+y2,z7;
M=I*freemodule(1);
CohenMacaulayTest1(M,3);


tst_status(1);$
