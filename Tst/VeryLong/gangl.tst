LIB "tst.lib";
tst_init();

//From: Herbert Gangl <herbert@mpim-bonn.mpg.de>
//Date: Tue, 22 Aug 2000 00:54:34 +0200 (MET DST)
//To: greuel@mathematik.uni-kl.de

//ring r = 0, (a(1..3),b(1..3),ai(1..3),bi(1..3)), dp;
echo =1;
ring r = 032003, (a(1..3),b(1..3),ai(1..3),bi(1..3)), dp;
option(prot);
ideal I; 
int i,ii; 

for(i=1; i<=3;i++) {I[2*i-1]=a(i)*ai(i)-1; I[2*i]=b(i)*bi(i)-1;}

poly f = b(3)*a(3)*(1-a(1))^2*(1-b(1))^2-a(3)*b(3)*(1-a(3))^2*(1-b(3))^2;

ideal J = (b(1)-b(3))*(1-a(1)*a(3))-b(2)*(1-b(1)*b(3))*(a(3)-a(1)), 
  (b(1)-a(3))*(b(3)-a(1))*a(2)-(b(1)*a(3)-1)*(b(3)*a(1)-1);


poly ff = b(2)*a(2)*(1-a(3))*(1-a(3))*(1-b(3))*(1-b(3))-a(2)*b(2)*(1-a(2))*(1-a(2))*(1-b(2))*(1-b(2));
I+J+f;

//ideal K = groebner(I+J+f);
ideal K = std(I+J+f);
size(K);

kill r;
tst_status(1);$
