LIB "tst.lib"; tst_init(); option(prot);

ring S=32003,(x,y,z,e,f,t,u,v,w,a,b,c,d,h),dp;
ideal J=
   f2+w2-1,
   x2+t2+a2-1,
   y2+u2+b2-1,
   z2+v2+c2-1,
   e2+d2-1,
   ft+wa,
   xy+tu+ab,
   yz+uv+bc,
   ze+cd,
   x+y+z+e+1,
   f+t+u+v-1,
   w+a+b+c+d;

J=homog(J,h);
//ideal L=std(J);
intvec hi=//hilb(L,1);
1,-3,-6,26,9,-99,24,224,-181,-211,682,-2234,5965,-9861,10380,-7236,3388,-1058,216,-28,2,0;
ideal K=eliminate(J,xyztuvwabcd,hi);

tst_status(1);$
