//test modul oprations, wrong in 0.8.2, 0.8.2a
ring r=32003,(x,y,z),(c,dp);
module m1=[x];
module m2=[y];
m1+m2;
m1*m2;
//
poly f=z;
f+m1;
f*m2;
//
typeof(f);
typeof(m1);
typeof(m1+m2);
typeof(m1[1]);
typeof(0);
m1[1]=0;
typeof(m1[1]);
// intersect for modules
m1[1]=[x];
intersect(m1,m2);
listvar(all);
// ideal sum: idCompactify is wrong
m1+m2;
ideal i1=x;
ideal i2=y;
i1+i2;
//
intersect(i1,i2);
// lead-command
f=x+z;
lead(f);
m1[3]=[f];
lead(m1);
lead(m1[1]);
lead(i1);
lead(i1+i2);
vector v=[f,1];
lead(v);
// homog-command
homog(f,z);
f=f+1;
homog(f,z);
homog(v,z);
m1[1]=v;
m1[2]=[f];
homog(m1,z);
i1=f,z+1;
homog(i1,z);
// accessing an ideal/modul as an "open" array
i1=1,2,3;
i1;
i1[15]=15;
i1;
m1[5]=[15];
m1;
$;
