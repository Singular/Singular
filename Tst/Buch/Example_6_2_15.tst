LIB "tst.lib";
tst_init();

proc mapIsFinite(R,map phi,ideal I)
{
   def S=basering;
   setring R;
   ideal ma=maxideal(1);
   setring S;
   ideal ma=phi(ma);
   ma=std(ma+I);
   if(dim(ma)==0)
   {
      return(1)
   }
   return(0)
}

ring A=0,(x,y),ds;
ring B=0,(x,y,z),ds;
map phi=A,x,y;
ideal I=z2-x2y;
mapIsFinite(A,phi,I);

tst_status(1);$
