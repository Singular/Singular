//deform_l.tst
//long  tests for deform.lib
//---------------------------
LIB "tst.lib";
tst_init();
LIB "deform.lib";
 printlevel =  2;
ring  r0  = 0,(x,y,z),ls;
 ideal i2 = y4-xy,yz-xz,z4-xz;   
 versal(i2);kill_rings();
//------------------------------------------------------------------------------
ring  r1  = 0,(x,y,z,u),dp;
 ideal i5 = x3-y2,xz,xu,yz,yu,z3-u2; 
 versal(i5);kill_rings();
 ideal i6 = z2-xz,zu-xu,u2-yu,yz-xu;
 versal(i6);kill_rings();
//------- cone over rational normal curve ofe degree d -------------------------
 int    d = 4;
ring r2   = 0,(x(1..d+1)),ls;
 ideal i7 = maxideal(1);
 matrix m[2][d] = i7[1..d],i7[2..d+1];
       i7 = minor(m,2);
       i7 = minbase(i7);
 versal(i7);kill_rings();
//------- L_d_d: d generic lines in A^d ________________________________________
        d = 4;
ring r3=0,(x(1..d)),ls;
 int n,m; ideal i8;
 for (n=1;n<d;n=n+1)
 { for (m=n+1;m<=d;m=m+1) {i8=i8,x(n)*x(m);}
 } 
        i8 = simplify(i8,10);
 versal(i8,3,"myring");kill_rings("myring");
//------- two elliptic singularity  (t1=7 and 13,t2=3) ---------------------------
ring r6   = 0,(x,y,z,u,v,w),ds;
 ideal i9 = uv-zw,u2-yw,zu-yv,yu-xw,xu+v2-uw,yz-xv,y2+v2-uw,xy+zv-yw,x2+z2-yu;
 versal(i9);kill_rings();
ring r7   = 0,(x,y,z,u,v),dp;
 ideal I1 = xy,xz,xu,xv,yz,yu,yv,zu,zv,uv;    
 versal(I1);kill_rings();
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
 versal(I3);kill_rings();
kill r0,r1,r2,r3,r6,r7,r8;
//////////////////////////////////////////////////////////////////////////////////
ring   Po = 0,(x,y),dp;
ideal  Io = std(x^4+y^3);
matrix Mo;
//============= rk 1 ======================================
module k =[x],[y];
module m(0)=k;
module m(1)=[x,y],[-y2,x3];
module m(2)=[x3,y],[-y2,x];
module m(3)=[x2,y],[-y2,x2];
module m(4)=[y,x,0],[-x2,0,-y],[0,-y,-x];
//============= rk 2 ======================================
module m(5)=[xy,-y2,x2],[-y2,-x3,-xy],[-x3,x2y,y2];
module m(6)=[xy,-yy,xx,0],[-yy,-xxx,-xy,0],[-xx,0,0,-y],[0,x2,y,x];
int i';
int ii=6;
for (i'=4;i'<ii;i'=i'+1)
{
  Mo=m(i');
  mod_versal(Mo,Io);
  kill_rings();
}
mod_versal(m(2),Io,4,"qq","W","dp","oo");
listvar(ring);
qqQx;
tst_status(1);$
