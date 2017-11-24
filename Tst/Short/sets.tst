LIB "tst.lib";
tst_init();

LIB "sets.lib";
printlevel=0;

 Set S0a = list(list(1,2,3),list(list(1,2)),list(10,11));
 Set S0b = list(list(10,11),list(list(1,2)));
 S0b<S0a;
 S0a<S0b;
 S0a==S0a;
 S0a==S0b;
 list L = 1,1,2,3;
 Set S1 = L;
 S1;
 ring R1;
 ring R2 = 0,(x,y),dp;
 Set S2 = list(R1,R1,R2);
 S2;
 ideal I1 = x+y;
 ideal I2 = y^2;
 ideal I3 = x+y, (x+y)^3;
 Set S3 = list(I1,I2,I3);
 S3;
 isEqual(I1,I3);
 isEqual(I1,I2);
 module M1 = x*gen(1), y*gen(1);
 module M2 = y^2*gen(2);
 module M3 = (x+y)*gen(1), (x-y)*gen(1);
 Set S4 = list(M1,M2,M3);
 S4;
 intmat m1[2][3]= 1,2,3,4,5,6;
 intmat m2[2][3]= 1,2,3,4,5,7;
 Set S5 = list(m1,m2,m1);
 S5;
 bigintmat b1[2][3]= 1,2,3,4,5,6;
 bigintmat b2[2][3]= 1,2,3,4,5,7;
 Set S6 = list(b1,b2,b1);
 S6;
 resolution r1 = res(maxideal(3),0);
 resolution r2 = res(maxideal(4),0);
 Set S7 = list(r1,r1,r2);
 size(S7);


 list l =1,2,3;
 list j =2,3,4;
 Set N=l;
 Set M=j;
 N;
 M;
 N*M;
 kill j;


 int i=1;
 int j=5;
 list k =1,2,3,4;
 Set M=k;
 i;
 j;
 M;
 isElement(i,M);
 isElement(j,M);
 kill M,k,j;



 list l =1,2;
 list j =1,2,3,4;
 Set N=l;
 Set M=j;
 N;
 M;
 complement(N,M);
 kill M,N,l,j;


 list l =1,2;
 list j =1,2,3,4;
 Set N=l;
 Set M=j;
 N;
 M;
 N<M;
 M<N;
 kill M,N,l,j;


 list l =1,2;
 list j =1,2,3,4;
 Set N=l;
 Set M=j;
 N;
 M;
 N>M;
 M>N;
 kill M,N,l,j;


tst_status(1);$

