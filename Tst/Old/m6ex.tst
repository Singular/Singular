  LIB "deform.lib";
  ring R=32003,(x,y,z),ds;
  //----------------------------------------------------
  // hypersurface case (from series T[p,q,r]):
  int p,q,r = 3,3,4;
  poly f = x^p+y^q+z^r+xyz;
  print(deform(f));
  // the miniversal deformation of f=0 is the projection from the
  // miniversal total space to the miniversal base space:
  // { (A,B,C,D,E,F,G,H,x,y,z) | x3+y3+xyz+z4+A+Bx+Cxz+Dy+Eyz+Fz+Gz2+Hz3 =0 }
  //  --> { (A,B,C,D,E,F,G,H) }
  //----------------------------------------------------
  // complete intersection case (from series P[k,l]):
  int k,l =3,2;
  ideal j=xy,x^k+y^l+z2;
  print(deform(j));
  list L=versal(j);                  // using default names
  def Px=L[1];
  setring Px;
  show(Px);                   // show is a procedure from inout.lib
  listvar(ideal);
  // ___ Equations of miniversal base space ___:
  Js;
  // ___ Equations of miniversal total space ___:
  Fs;
kill L;
  // the miniversal deformation of V(j) is the projection from the
  // miniversal total space to the miniversal base space:
  // { (A,B,C,D,E,F,x,y,z) | xy+A+Bz=0, y2+z2+x3+C+Dx+Ex2+Fy=0 }
  //  --> { (A,B,C,D,E,F) }
  //----------------------------------------------------
  // general case (cone over rational normal curve of degree 4):
  ring r1=0,(x,y,z,u,v),ds;
  matrix m[2][4]=x,y,z,u,y,z,u,v;
  ideal i=minor(m,2);                 // 2x2 minors of matrix m
  // Def_r will be the name of the miniversal base space with
  // parameters A(1),...,A(4)
  versal(i,0,"Def_r","A(");
  // the miniversal deformation of V(i) is the projection from the
  // miniversal total space to the miniversal base space:
  // { (A(1..4),x,y,z,u,v) |
  //         -y^2+x*z+A(2)*x-A(3)*y=0, -y*z+x*u-A(1)*x-A(3)*z=0,
  //         -y*u+x*v-A(3)*u-A(4)*z=0, -z^2+y*u-A(1)*y-A(2)*z=0,
  //         -z*u+y*v-A(2)*u-A(4)*u=0, -u^2+z*v+A(1)*u-A(4)*v=0 }
  //  --> { A(1..4) |
  //         -A(1)*A(4) = A(3)*A(4) = -A(2)*A(4)-A(4)^2 = 0 }
  //----------------------------------------------------
LIB "tst.lib";tst_status(1);$
