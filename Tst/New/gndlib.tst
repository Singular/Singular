LIB "tst.lib"; tst_init();

LIB "GND.lib";

proc doit(int ex)
{

//Example 1
//Example 7 from paper
if(ex == 1)
{
  ring All = 0,(a1,a2,a3,x1,x2,x3,Y1,Y2,Y3),dp;
  int nra = 3;
  int nrx = 3;
  int nry = 3;
  ideal xid = x2^3-x3^2,x1^3-x3^2;
  ideal yid = Y1^3-Y2^3;
  ideal aid = a3^2-a1*a2;
  poly y;
  int i;
  for(i=0;i<=30;i++)
  {
    y = y + a1*x3^i/factorial(i);
  }
  for(i=31;i<=50;i++)
  {
    y = y + a2*x3^i/factorial(i);
  }
  ideal f = a3*x1,a3*x2,y;
  desingularization(All, nra,nrx,nry,xid,yid,aid,f,"debug");
}

//Example 2
//Example 10 from paper
if(ex == 2)
{
  ring All = 0,(a1,a2,a3,a4,x1,x2,Y1,Y2,Y3,Y4),dp;
  int nra = 4;
  int nrx = 2;
  int nry = 4;
  ideal xid = x1^2-x2^3;
  ideal yid = Y3^2-x1^2*Y1*Y2,Y4^2-x2*Y2*Y3;
  ideal aid = 0;
  poly y1,y2,y3,y4;
  y3 = 1+a1*x2+a1*x2^2+a3*x1*x2^4+a4*x1*x2^6;
  y4 = 1+a4*x2+(a2^2+a3)*x2^10+(a1+a2+a3+a4)*x1*x2^11;
  
  list as,xs;
  if(nra != 0)
  {
    for(int i=1;i<=nra;i++)
    {
      as[size(as)+1] = string(var(i));
    }
  }
  if(nrx!=0)
  {
    for(int i=nra+1;i<=nra+nrx;i++)
    {
      xs[size(xs)+1] = string(var(i));
    }
  }
  y1 = y3^3*invp(y4^2,12,as,xs);
  y2 = y4^2*invp(y3,12,as,xs);
  y3 = x2*y3;
  y4 = x2*y4;
  ideal f = y1,y2,y3,y4;
  desingularization(All, nra,nrx,nry,xid,yid,aid,f,"injective","debug");
}

//Example 3

if(ex == 3)
{
  ring All = 0,(a1,a2,x1,x2,Y1,Y2,Y3),dp;
  int nra = 2;
  int nrx = 2;
  int nry = 3;
  ideal xid = x1^2-x2^3;
  ideal yid = Y3^2-x1^2*Y1*Y2;
  ideal aid = 0;
  poly y1,y2,y3;
  y2 = 1+(a1+a2)*x2^2;
  y3 = 1+a1*x1+a2*x2^2;
  list as,xs;
  if(nra != 0)
  {
    for( int i=1;i<=nra;i++)
    {
      as[size(as)+1] = string(var(i));
    }
  }
  if(nrx!=0)
  {
    for(int i=nra+1;i<=nra+nrx;i++)
    {
      xs[size(xs)+1] = string(var(i));
    }
  }
  y1 = y2^2*invp(y3,10,as,xs);
  ideal f = y1,y2,y3;
  //desingularization(All, nra,nrx,nry,xid,yid,aid,f,"injective");
  desingularization(All, nra,nrx,nry,xid,yid,aid,f,"injective","debug");
}

//Example 4

if(ex == 4)
{
  ring All = 0,(a1,a2,a3,a4,x,Y1,Y2,Y3),dp;
  int nra = 4;
  int nrx = 1;
  int nry = 3;
  ideal xid = 0;
  ideal yid = Y1^3-Y2^3;
  ideal aid = a3^2-a1*a2,a4^2+a4+1;
  poly y;
  int i;
  for(i=0;i<=30;i++)
  {
    y = y + a1*x3^i/factorial(i);
  }
  for(i=31;i<=50;i++)
  {
    y = y + a2*x3^i/factorial(i);
  }
  ideal f = a3*x,a3*a4*x,y;
  desingularization(All, nra,nrx,nry,xid,yid,aid,f,"debug");
}

//Example 5
// Example 4 from paper

if(ex == 5)
{
  ring All = 0,(a1,a2,a3,x1,x2,Y1,Y2,Y3),dp;
  int nra = 3;
  int nrx = 2;
  int nry = 3;
  ideal xid = x1^3-x2^2;
  ideal yid = Y1^3-Y2^3;
  ideal aid = a3^2+a3+1;
  poly y;
  int i;
  for(i=0;i<=30;i++)
  {
    y = y + a1*x1^i/factorial(i);
  }
  for(i=31;i<=50;i++)
  {
    y = y + a2*x2*x1^i/factorial(i);
  }
  ideal f = a1*x2,a1*a3*x2,y;
  desingularization(All, nra,nrx,nry,xid,yid,aid,f,"debug");
}

if(ex==6)
{
  ring All = 0,(a1,a2,a3,x1,x2,Y1,Y2,Y3),dp;
  int nra = 3;
  int nrx = 2;
  int nry = 3;
  ideal xid = x1^3-x2^2;
  ideal yid = Y1^3-Y2^3;
  ideal aid = a3^2+a3+1;
  poly y;
  int i;
  for(i=0;i<=30;i++)
  {
    y = y + a1*x1^i/factorial(i);
  }
  for(i=31;i<=50;i++)
  {
    y = y + a2*x2*x1^i/factorial(i);
  }
  ideal f = a1*x2,a1*a3*x2,y;
  desingularization(All, nra,nrx,nry,xid,yid,aid,f,"debug");
}


//Example 7
//Example 10 is smaller
if(ex == 7)
{
  ring All = 0,(a1,a2,a3,a4,x1,x2,Y1,Y2,Y3,Y4),dp;
  int nra = 4;
  int nrx = 2;
  int nry = 4;
  ideal xid = x1^2-x2^3;
  ideal yid = Y3^2-x1^2*Y1*Y2,Y4^2-x2*Y2*Y3;
  ideal aid = 0;
  poly y1,y2,y3,y4;
  y3 = 1+a1*x2+(a2+a3+a4)*x2^2;
  y4 = 1+a2*x2^2;
  
  list as,xs;
  if(nra != 0)
  {
    for( int i=1;i<=nra;i++)
    {
      as[size(as)+1] = string(var(i));
    }
  }
  if(nrx!=0)
  {
    for(int i=nra+1;i<=nra+nrx;i++)
    {
      xs[size(xs)+1] = string(var(i));
    }
  }
  y1 = y3^3*invp(y4^2,12,as,xs);
  y2 = y4^2*invp(y3,12,as,xs);
  y3 = x2*y3;
  y4 = x2*y4;
  ideal f = y1,y2,y3,y4;
  desingularization(All, nra,nrx,nry,xid,yid,aid,f,"injective","debug");
}

//Example 8
//Example 17 from paper
if(ex == 8)
{
  ring All = 0,(a1,a2,x1,x2,Y1,Y2,Y3,Y4),dp;
  int nra = 2;
  int nrx = 2;
  int nry = 4;
  ideal xid = x1^2-x2^3;
  ideal yid = Y3^2-x1^2*Y1*Y2,Y4^2-x2*Y2*Y3;
  ideal aid = 0;
  poly y1,y2,y3,y4;
  y3 = 1+a1*x2;
  y4 = 1+a2*x2^2;
  
  list as,xs;
  if(nra != 0)
  {
    for( int i=1;i<=nra;i++)
    {
      as[size(as)+1] = string(var(i));
    }
  }
  if(nrx!=0)
  {
    for(int i=nra+1;i<=nra+nrx;i++)
    {
      xs[size(xs)+1] = string(var(i));
    }
  }
  y1 = y3^3*invp(y4^2,12,as,xs);
  y2 = y4^2*invp(y3,12,as,xs);
  y3 = x2*y3;
  y4 = x2*y4;
  ideal f = y1,y2,y3,y4;
  desingularization(All, nra,nrx,nry,xid,yid,aid,f,"injective","debug");
}

//Example 9
// Example 4 with not injective from paper (Example 5)

if(ex == 9)
{
  ring All = 0,(a1,a2,a3,x1,x2,Y1,Y2,Y3),dp;
  int nra = 3;
  int nrx = 2;
  int nry = 3;
  ideal xid = x1^3-x2^2;
  ideal yid = Y1^3-Y2^3;
  ideal aid = a3^2+a3+1;
  poly y;
  int i;
  for(i=0;i<=30;i++)
  {
    y = y + a1*x1^i/factorial(i);
  }
  for(i=31;i<=50;i++)
  {
    y = y + a2*x2*x1^i/factorial(i);
  }
  ideal f = a1*x2,a1*a3*x2,y;
  desingularization(All, nra,nrx,nry,xid,yid,aid,f,"injective","debug");
}
}

int ii;

for(ii=1;ii<=9;ii++)
{
  //otherwise it would take to much time and the output will be HUGE
  if(ii != 2 && ii!= 3 && ii != 7 && ii!= 8)
  {
    doit(ii);
  }
}

tst_status(1);$
