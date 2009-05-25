LIB "tst.lib";
tst_init();

// homogonizations and variable weights:

ring r=0,(x,y,z),M(2,3,1,1,0,0,0,1,0);
poly p=x+y+z;
homog(p,z);
ring s=0,(x,y,z),wp(2,3,1);
poly p=x+y+z;
homog(p,z);

ring rr=0,(x,y,z),M(2,3,4,1,0,0,0,1,0);
poly p=x+y+z;
homog(p,z);
ring ss=0,(x,y,z),wp(2,3,4);
poly p=x+y+z;
homog(p,z);

// -- syz in qring (fixed in 3-1-0-3)
ring rs=32003,(T(1..11)),(a(1,1,1,1,1,1,1,2,2,1,2),dp,C);
ideal I;
I[1]=T(3)*T(7)-T(2)*T(10);
I[2]=T(6)^2-T(2)*T(10);
I[3]=T(5)*T(6)+T(7)^2;
I[4]=T(3)*T(6)+T(1)*T(10);
I[5]=T(2)*T(6)+T(1)*T(7);
I[6]=T(1)*T(6)-T(4)*T(10);
I[7]=T(3)*T(5)+T(6)*T(7);
I[8]=T(1)*T(5)-T(2)*T(7);
I[9]=T(2)*T(3)+T(4)*T(10);
I[10]=T(2)^2+T(4)*T(7);
I[11]=T(1)*T(2)-T(4)*T(6);
I[12]=T(1)^2+T(3)*T(4);
I[13]=T(8)*T(10)-T(7)*T(11);
I[14]=T(3)*T(8)-T(2)*T(11);
I[15]=T(10)^3+T(6)*T(11);
I[16]=T(7)*T(10)^2+T(6)*T(8);
I[17]=T(6)*T(10)^2+T(2)*T(11);
I[18]=T(5)*T(10)^2-T(7)*T(8);
I[19]=T(3)*T(10)^2-T(1)*T(11);
I[20]=T(2)*T(10)^2-T(1)*T(8);
I[21]=T(1)*T(10)^2+T(4)*T(11);
I[22]=T(6)*T(7)*T(10)+T(2)*T(8);
I[23]=T(1)*T(7)*T(10)+T(4)*T(8);
I[24]=T(7)^3+T(9)*T(10);
I[25]=T(6)*T(7)^2+T(2)*T(5)*T(10);
I[26]=T(2)*T(7)^2+T(3)*T(9);
I[27]=T(1)*T(7)^2+T(4)*T(5)*T(10);
I[28]=T(4)*T(5)*T(7)-T(1)*T(9);
I[29]=T(2)*T(5)*T(7)-T(6)*T(9);

I = std(I);
size(I);
qring q = I;
ideal f;
f[1]=T(10);
f[2]=T(3)*T(11);
f[3]=T(3)^2*T(4);
homog(f);
option(prot);
size(syz(f));
syz(f);

tst_status(1);$
