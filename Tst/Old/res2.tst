// betti wrong after minres (0.8.9f) (pfister)
ring rh3=32003,(w,x,y,z),(dp,C);
int a =11;
int b =10;
int c =3;
int t =1;
poly f= x^a+y^b+z^(3*c)+x^(c+2)*y^(c-1)+x^(c-1)*y^(c-1)*z3+x^(c-2)*y^c*(y2+t*x)^2;
ideal j= jacob(f);
ideal i=homog(j,w);
//timer=1;
ideal j1=std(i);
ideal j0;
int k;
int l=size(j1);
for (k=1;k<=l;k=k+1)
{
   j0[k]=j1[l-k+1];
}
j0;
attrib(j0,"isSB",1);
list jres=sres(j0,0);
jres[3];
list jmin=minres(jres);
jmin[3];
betti(jmin);
kill a,b,c,t,rh3;
$   
