//
//


LIB "tst.lib";

LIB "polylib.lib";

tst_init();

proc char_out(ideal i){
  return(sort(simplify(lead(i),1))[1]);
}
ring r=0,x(1..5),dp;
ideal i=cyclic(5);
char_out(slimgb(i));
kill r;

ring r=7,x(1..5),dp;
ideal i=cyclic(5);
char_out(slimgb(i));
kill r;

ring r = 32003, (x,y,z,t,u,v,w,a,b,c,d,e,f,g,h,i,j,k), dp;
ideal i0=
ag ,
df+bg+ah+w ,
ci ,
wf ,
cg+ai+di+ei+cj+t+c ,
cf+ag+dg+eg+ch+bi+aj+dj+ej+y+u+v+2w+a+d+e ,
af+df+ef+wg+bg+ah+dh+eh+wj+bj+x+z+w+b-1 ,
wf+bf+wh+bh ,
2wf+bf+wh ,
2af+df+ef+2wg+bg+ah+wj+x+w ,
cf+2ag+dg+eg+wi+aj+y+a ,
cg+ai ,
af+wg ,
bf+wh+2bh ,
df+bg+ah+2dh+eh+bj+z+w+2b ,
dg+ch+bi+dj+u+a+2d+e ,
di+c ,
bf+wh ,
dg+a;
ideal i=homog(i0,k);
i = slimgb(i);
char_out(i);
kill r;

ring @r=(32003,u1, u2, u3, u4),(x1, x2, x3, x4, x5, x6, x7),dp;
ideal @i=
-x4*u3+x5*u2,
x1*u3+2*x2*u1-2*x2*u2-2*x3*u3-u1*u4+u2*u4,
-2*x1*x5+4*x4*x6+4*x5*x7+x1*u3-2*x4*u1-2*x4*u4-2*x6*u2-2*x7*u3+u1*u2+u2*u4,
-x1*x5+x1*x7-x4*u1+x4*u2-x4*u4+x5*u3+x6*u1-x6*u2+x6*u4-x7*u3,
-x1*x4+x1*u1-x5*u1+x5*u4,
-2*x1*x3+x1*u3-2*x2*u4+u1*u4+u2*u4,
x1^2*u3+x1*u1*u2-x1*u2^2-x1*u3^2-u1*u3*u4+u3*u4^2;
@i=slimgb(@i);
char_out(@i);
kill @r;

ring r=0,(x,y,z),lp;
ideal i=x3-y,xy-z,x2-z6,yz-z7,xz-y;
char_out(slimgb(i));
kill r;


//twomat3
ring @r=32003,(a, b, c, d, e, f, g, h, i, A, B, C, D, E, F, G, H, I),lp;
ideal @i=
d*C-c*D-f*E+e*F-i*F+f*I,
-d*B-g*C+b*D+c*G,
d*B-b*D-h*F+f*H,
g*A+h*D-a*G+i*G-d*H-g*I,
d*A-a*D+e*D-d*E-g*F+f*G,
-c*A-f*B+a*C-i*C+b*F+c*I,
-b*A+a*B-e*B-h*C+b*E+c*H,
-g*h*C*I+g*B+h*E+h*F-b*G-c*G-e*H-f*H+i*H;
char_out(slimgb(@i));
kill @r;

tst_status(1);$
