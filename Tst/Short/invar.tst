LIB "tst.lib";
tst_init();

LIB"invar.lib";

  ring rw=0,(x(1..5)),dp;
  matrix m[5][1];
  m[3,1]=x(1);
  m[4,1]=x(2);
  m[5,1]=1+x(1)*x(4)+x(2)*x(3);
  ideal in=invariantRing(m,x(3),x(1),0);
  in;
kill rw;
  ring rf=0,(x(1..7)),dp;
  matrix m[7][1];
  m[4,1]=x(1)^3;
  m[5,1]=x(2)^3;
  m[6,1]=x(3)^3;
  m[7,1]=(x(1)*x(2)*x(3))^2;
  ideal in=invariantRing(m,x(4),x(1),6);
  in;
kill rf;
  ring rd=0,(x(1..5)),dp;
  matrix m[5][1];
  m[3,1]=x(1);
  m[4,1]=x(2);
  m[5,1]=1+x(1)*x(4)^2;
  ideal in=invariantRing(m,x(3),x(1));
  in;
  
  actionIsProper(m);
  
  //computes the relations between the invariants
  int z=size(in);
  ideal null;
  ring r1=0,(y(1..z)),dp;
  setring rd;
  map phi=r1,in;
  setring r1;  
  ideal ker=preimage(rd,phi,null); 
  ker;
kill rd,r1;
  int n=5;
  
  ring w=0,(x(1..n)),wp(1..n);
  
  // definition of the vectorfield m=sum m[i]*d/dx(i)
  matrix m[n][1];
  int i;
  for (i=1;i<=n-1;i=i+1)
  {
    m[i+1,1]=x(i);
  }
  ideal in=invariantRing(m,x(2),x(1),0);
  in;
kill w;
tst_status(1);$
