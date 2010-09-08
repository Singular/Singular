//LIB "tst.lib";
tst_init();

LIB"resjung.lib";
int i,k;

ring r=0,(x,y,z),dp;
ideal I=z2-x3y3;
list li=jungresolve(I,0);
k=size(li);
for(i=1;i<=k;i++)
{
"---> ring ",i,":";
  def S=li[i];
  setring S;
  showBO(BO);
  setring r;
  kill S;
}

kill r;
ring r=0,(x,y,z),dp;
ideal I=z2-x3-y3;
list li=jungresolve(I,0);
k=size(li);
for(i=1;i<=k;i++)
{
"---> ring ",i,":";
  def S=li[i];
  setring S;
  showBO(BO);
  setring r;
  kill S;
}

kill r;
ring r=0,(x,y,z),dp;
ideal I=z5-x2-y3;
list li=jungresolve(I,0);
k=size(li);
for(i=1;i<=k;i++)
{
"---> ring ",i,":";
  def S=li[i];
  setring S;
  showBO(BO);
  setring r;
  kill S;
}

kill r;
ring r=0,(x,y,z,v),dp;
ideal I=x2-yz,z2-yv;
list li=jungresolve(I,0);
k=size(li);
for(i=1;i<=k;i++)
{
"---> ring ",i,":";
  def S=li[i];
  setring S;
  showBO(BO);
  setring r;
  kill S;
}
