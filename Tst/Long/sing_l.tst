//sing_l.tst
//long  tests for sing.lib
LIB "tst.lib";
tst_init();
LIB "sing.lib";
ring r    = 0,(x,y,z),ds;
   ideal i  = xy,xz,yz;
   matrix T = deform(i);
   print(T);
   print(deform(x3+y5+z2));
//pause;
   i = x5+y6+z6,x2+2y2+3z2;
   dim_slocus(i);
//pause;
   i  = yx3+y,yz3+y3z;
   poly f   = x;
   is_active(f,i);
   qring q  = std(x4y5);
   poly f   = x;
   module m = [yx3+x,yx3+y3x];
   is_active(f,m);
   setring r; kill q;
//pause;
   printlevel = 1;  
   i  = x4+y5+z6,xyz,yx2+xz2+zy7;
   is_ci(i);
   i  = xy,yz;
   is_ci(i);
//pause;
   i    = x2y,x4+y5+z6,yx2+xz2+zy7;
   is_is(i);
   f     = xy+yz;
   is_is(f);
//pause;
   ideal i1 = x8,y8,z;
   ideal j1 = (x+y)^4,z;
   i       = intersect(i1,j1);
   f  = xy;
   is_reg(f,i);
//pause;
   i    = x8,y8,(x+y)^4;
   is_regs(i);
   module m   = [x,0,y];
   i          = x8,(x+z)^4;
   is_regs(i,m);
//pause;
   ideal j    = x5+y6+z6,x2+2y2+3z2,xyz+yx;
   milnor(j);
   f     = x7+y7+(x-y)^2*x2y2+z2;
   milnor(f);
//pause;  
   i = x5+y6+z6,x2+2y2+3z2;
   slocus(i);
//pause;
   f=x3+y3+z4;
   intvec w=4,4,3;
   qhspectrum(f,w);
//pause;
   kill r;
   ring r=0,(x,y,z),(c,ds);
   ideal j=x2+y2+z2,x2+2y2+3z2;
   tjurina(j);
//pause;   
   Tjurina(j);
//pause;   
   Tjurina(j,1);
   ideal i    = xy,xz,yz;
   module T   = T1(i);
   vdim(T);                   "// Tjurina number = dim_K(T1), should be 3";
//pause;
   list L=T1(i,"");
   print(transpose(L[3])*L[2]); "// should be 0 (mod i)";
   L;
//pause; 
   kill r;   
   ring  r    = 0,(x,y),(c,dp);
   ideal j    = x6-y4,x6y6,x2y4-x5y2;
   module T   = T2(j);
   vdim(T);
   hilb(T);"";
//pause;
   T2(j,1);
   ring r1    = 0,(x,y,z),dp;
   ideal id   = xy,xz,yz;
   list L     = T2(id,"");
   vdim(L[1]);                        
//pause;
   L;
   ring r2     = 0,(x,y,z,u,v),(c,ds);
   ideal i    = xz-y2,yz2-xu,xv-yzu,yu-z3,z2u-yv,zv-u2;
   list L     = T12(i,1);
//pause;
    L;
    kill r1,r2;
//
ring  r1  = 0,(x,y,z,u),dp;
ideal i6 = z2-xz,zu-xu,u2-yu,yz-xu; 
list L=T12(i6,1);
L;
kill r1;
//
ring r7   = 0,(x,y,z,u,v),dp;
 ideal I1 = xy,xz,xu,xv,yz,yu,yv,zu,zv,uv;    " //t1=15, t2=20";
 list L=T12(I1,1);
 L;
 kill r7;
//
ring r8   = 0,(w(1..9)),dp;
 ideal I3 = w(8)^2-w(6)*w(9),
            w(7)*w(8)-w(5)*w(9),
            w(5)*w(8)-w(3)*w(9),
            w(4)*w(8)-w(2)*w(9),
            w(7)^2-w(4)*w(9),
            w(6)*w(7)-w(3)*w(9),
            w(5)*w(7)-w(2)*w(9),
            w(4)*w(7)-w(1)*w(9),
            w(3)*w(7)-w(2)*w(8),
            w(2)*w(7)-w(1)*w(8),
            w(5)*w(6)-w(3)*w(8),
            w(4)*w(6)-w(2)*w(8),
            w(5)^2-w(3)*w(7),
            w(4)*w(5)-w(2)*w(7),
            w(3)*w(5)-w(2)*w(6),
            w(2)*w(5)-w(1)*w(6),
            w(4)^2-w(1)*w(7),
            w(3)*w(4)-w(2)*w(5),
            w(2)*w(4)-w(1)*w(5),
            w(2)^2-w(1)*w(3),
            w(2)*w(6)*w(8)-w(3)^2*w(9),
            w(1)*w(6)*w(8)-w(2)*w(3)*w(9),
            w(2)*w(6)^2-w(3)^2*w(8),
            w(1)*w(6)^2-w(2)*w(3)*w(8);
T12(I3);
tst_status(1);$
