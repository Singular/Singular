LIB "tst.lib";
tst_init();

LIB"primdec.lib";
option(prot);

ring r=32003,(a,b,c,d,e,f,g,h,j),(C,dp);
ideal i=
a+2b+c-d+g,
f2gh-a,
efg-c,
fg2j-b,
a+b+c+f+g-1,
3ad+3bd+2cd+df+dg-a-2b-c-g;

list pr1=minAssPrimes(i);
ideal i1=radical(i);
ideal i2=equiRadical(i);
list pr2= primdecGTZ(i);
list pr3=min_ass_prim_charsets(i,1);
list pr4=primdecSY(i,1);
matrix m=char_series(i);
pr1;
i1;
i2;
pr2;
pr3;
pr4;
m;
testPrimary(pr2,i);
kill r;

ring r=32003,(b,s,t,u,v,w,x,y,z),(C,dp);
ideal i=
bv+su,
bw+tu,
sw+tv,
by+sx,
bz+tx,
sz+ty,
uy+vx,
uz+wx,
vz+wy;

list pr1=minAssPrimes(i);
ideal i1=radical(i);
ideal i2=equiRadical(i);
list pr2= primdecGTZ(i);
list pr3=min_ass_prim_charsets(i,1);
list pr4=primdecSY(i,1);
matrix m=char_series(i);
pr1;
i1;
i2;
pr2;
pr3;
pr4;
m;
testPrimary(pr2,i);
kill r;
ring r=0,(b,s,t,u,v,w,x,y,z),(C,dp);
ideal i=
bv+su,
bw+tu,
sw+tv,
by+sx,
bz+tx,
sz+ty,
uy+vx,
uz+wx,
vz+wy;

list pr1=minAssPrimes(i);
ideal i1=radical(i);
ideal i2=equiRadical(i);
list pr2= primdecGTZ(i);
list pr3=min_ass_prim_charsets(i,1);
list pr4=primdecSY(i,1);
matrix m=char_series(i);
pr1;
i1;
i2;
pr2;
pr3;
pr4;
m;
testPrimary(pr2,i);
kill r;

ring r= 32003,(x,y,z),(C,dp);
ideal i=
17y6+49y5-9y4+41x2z+12y3+33y2+11y+73,
42y5+33y4+21x2y+17x2z+63y3+11xy+77y2+91y+1,
26y2+44xz+12y+9;

list pr1=minAssPrimes(i);
ideal i1=radical(i);
ideal i2=equiRadical(i);
list pr2= primdecGTZ(i);
list pr3=min_ass_prim_charsets(i,1);
list pr4=primdecSY(i,1);
matrix m=char_series(i);
pr1;
i1;
i2;
pr2;
pr3;
pr4;
m;
testPrimary(pr2,i);
kill r;

ring r =32003,(a,b,c,d,e,h),(C,dp);
ideal i=
a+b+c+d+e,
de+1cd+1bc+1ae+1ab,
cde+1bcd+1ade+1abe+1abc,
bcde+1acde+1abde+1abce+1abcd,
abcde-h5;

list pr1=minAssPrimes(i);
ideal i1=radical(i);
ideal i2=equiRadical(i);
list pr2= primdecGTZ(i);
list pr3=min_ass_prim_charsets(i,1);
list pr4=primdecSY(i,1);
matrix m=char_series(i);
pr1;
i1;
i2;
pr2;
pr3;
pr4;
m;
testPrimary(pr2,i);
kill r;
$
