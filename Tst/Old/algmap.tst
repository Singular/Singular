ring r0=(0,a,b),(x,y,z),dp;
ideal i0=
(3a2b-2b5)*x2z+(1000b+a20)*xy2,
(20b5+a4b2)*xz3+x2yz,
xy3+x3y,
(20b10)*xyz2+yz4,
xyz+(a+b)*z2;
i0;

ring r1=0,(x,y,z,a,b),dp;
ideal i=a,b;
ideal j=x,y,z;
ideal i1=system("algmap",r0,i0,i,j);
i1;

i1=system("algfetch",r0,i0);
i1;

ring r2=(0,a),(x,y,z,b),dp;
ideal i=a,b;
ideal j=x,y,z;
ideal i1=system("algmap",r0,i0,i,j);
i1;

i1=system("algfetch",r0,i0);
i1;
kill r2;

ring r2=(0,a,b,z),(x,y),dp;
ideal i=a,b;
ideal j=x,y,z;
ideal i1=system("algmap",r0,i0,i,j);
i1;

i1=system("algfetch",r0,i0);
i1;
kill r2;

LIB "tst.lib";tst_status(1);$

