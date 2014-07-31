LIB "tst.lib";

tst_init();

ring r=0,(x4,x3,x2,x1,x5,y),dp;
poly f2=((y+x1)*(y-2*x4)*(y+x2+x3));
ideal I= x1^2+x3*x1-x1*x4+x2^2-x2+x3*x4-x4^2-x4,x1^2+x2*x1-x1*x4+x2^2+x3*x2+x2+x4^2-x4
          , 1+x1*x2-x1*x3+x1*x4+x1+x2^2-x3*x2+x2*x4-x2+x3^2+x3*x4-x4^2 , x1^2+x1*x2+x3*x1+x1*x4-x2^2+x3*x2-x2+x3^2-x3*x4-x4^2+x4;
ideal J=I,f2;
char_series (J);

kill r;

ring r=0,(x1,x2,x3,x4,x5,y),dp;
poly f3=((y-2*x4^2+x3*x1+x2+1)*(y+x2^2+x3*x4+x1*x3+2));
ideal I=-1-x1^2+x3*x1+x2^2-x3*x2+x3^2-x3*x4+x4^2+x4 , 1+x2*x1+x3*x1+x1-x3*x2-x2*x4+x2-x3^2-x3*x4-x3 , 1+x1*x3+x1*x4+x1+x2^2+x2*x4-x2-x3-x4^2 , 
     x1^2+x1*x2+x3*x1+x1*x4-x1-x2*x4-x3^2+x3*x4-x3+x4^2+x4;
ideal J=I,f3;
char_series (J);

//examples from J. Kroeker
 ring rng = 0,(xz,xs,xj),(lp(1),lp(2));
ideal I = -93*xz^2*xs*xj^2+93*xz*xs*xj^2-130*xs^3*xj^2,
 82*xz^3*xs*xj+90*xz*xs^2*xj^2-66*xs;
char_series(I);

kill rng;

ring rng = 0,(xt,xl,xy),(lp(1),lp(2));
ideal I = 45*xt*xy^4+53,
84*xt^2*xy^3-84*xt*xy^3-105*xl*xy;
char_series(I);

kill rng;

ring rng = 0,(xr,xd,xs),(lp(1),lp(2));
ideal I = -124*xr^2-9*xr*xd*xs^2,
62*xr^2*xs^2-62*xr*xs^2+126*xd;
char_series(I);

kill rng;

ring rng = 0,(xm,xs,xg),(lp(1),lp(2));
ideal I = 51*xm^2-51*xm-130*xs^2,
-27*xm^2*xs*xg^2+49*xs*xg^2-99;
char_series(I);

kill rng;

ring rng = 0,( xv,xg,xq  ),(lp(1),lp(2));
ideal I = -29*xv*xq^4-103*xg,
24*xv-105*xg^3*xq^2-24;
char_series(I);

kill rng;

ring rng = 0, (xv, xw ,xi, xk, xn),(lp(3),lp(2));
ideal I = 32*xv^2*xk-29*xw*xk-32*xk,
99*xw*xi^2+36*xw*xk^2,
55*xw*xk-26*xw*xn^2,
-44*xv*xw*xk+56*xi^2+134;
char_series(I);

kill rng;

ring rng = 0, ( xu ,xm, xl ,xr ,xj ),(lp(3),lp(2));
ideal I = 46*xu^2*xr-86*xl^2*xj+75,
-96*xj^2-91,
-21*xu*xr+7*xm+61*xr^2*xj;
char_series(I);

kill rng;

ring rng = 0, ( xk, xg, xl ,xt ,xf ),(lp(3),lp(2));
ideal I = -39*xk^2+99*xg*xl^2,
105*xl*xf^2+194,
96*xl*xf^2-14*xt+20*xf;
char_series(I);

kill rng;

ring rng = 0,( xc, xk, xq , xx,xz ),(lp(3),lp(2));
ideal I = 7*xc*xk-123*xc*xz^2,
132*xc*xq-43*xc*xx*xz+40*xc,
34*xk*xx+115,
49*xc*xk^2-32*xk^2-122*xk*xx^2;
char_series(I);

kill rng;

ring rng = 0,( xy, xz, xs,  xe, xh),(lp(3),lp(2));
ideal I = 78*xz^2+86*xe,
118*xy*xe^2-162,
34*xz*xs+20*xz*xh-136,
-119*xy^2*xz-27*xh^3-62;
char_series(I);

kill rng;

ring rng = 0,( xn, xd, xi,  xy, xf ),(lp(3),lp(2));
ideal I = -60*xn*xi-104*xd*xi+87*xf^2,
-34*xn*xd*xi+118,
109*xn^2*xi+124*xn*xd,
-57*xi*xy^2+47;
char_series(I);

kill rng;

ring rng = 0,(  xg, xc, xs, xo, xz),(lp(3),lp(2));
ideal I = 94*xg*xc^2+22,
-129*xg*xo+138*xs*xz^2-54,
-81*xo*xz^2+102,
27*xo^3-86;
char_series(I);

kill rng;

ring rng = 0,(   xa, xb, xq, xt ,xc ),(lp(3),lp(2));
ideal I = -34*xa^2+177,
-52*xa*xt^2-79*xb,
-24*xb*xq*xt-62,
138*xa*xc^2+117*xt;
char_series(I);

kill rng;

ring rng = 0,(   xu, xi, xw, xe, xr ),lp;
ideal I = -133*xi*xw-115*xw^2*xr,
-104*xw*xr^2-19*xw-17,
126*xu*xi^2-8,
94*xi*xe^2-100;
char_series(I);

kill rng;

ring rng = 0,(  xd, xx, xb, xj ,xt),lp;
ideal I = 30*xx*xj*xt+125*xx+36*xj*xt^2,
-36*xx+29*xb^2-79*xb,
42*xd*xx^2+65*xd*xx-76;
char_series(I);

kill rng;

ring rng = 0,(   xu ,xy, xm, xc, xs ),lp;
ideal I = 23*xu*xc*xs-40,
-138*xy*xm^2-97,
-20*xu*xy*xc-30*xy*xs-131*xm^2*xs,
-51*xu*xm^2-87*xs^2-61;
char_series(I);

kill rng;

ring rng = 0,( xb, xm, xc, xo, xw ),lp;
ideal I = -99*xb*xw^2-82,
-136*xb^2*xo+77*xc^2-44*xw,
-100*xm^2*xw-103*xm*xo^2-16;
char_series(I);

kill rng;

ring rng = 0,( xm ,xo, xi ,xk ,xn ),lp;
ideal I = -19*xo*xk^2-12*xn^3+114*xn,
-131*xm*xi-70*xi*xn^2,
-77*xm*xk*xn+28*xo+90*xi^2*xn;
char_series(I);

kill rng;

ring rng = 0,(  xz, xq, xa, xc, xl),lp;
ideal I = -56*xz*xc+16*xq*xl^2+132,
-6*xz-50*xq*xl+95*xc*xl,
138*xa^2+22*xc-37*xl;
char_series(I);

kill rng;

ring rng = 0,(  xz, xv, xb, xu, xp),lp;
ideal I = 100*xz*xb+95*xv^2*xp,
14*xz-31*xv*xb*xp,
-44*xz*xb*xp+43*xv^2-140*xu^2,
-40*xz^2*xb-xp;
char_series(I);

kill rng;

ring rng = 0,(  xp, xr, xt, xq, xb),lp;
ideal I = 64*xp-35*xr*xq-65*xt*xq^2,
-39*xr^2*xt+85*xt*xb^2+9*xb^3,
-91*xp*xt-130,
-60*xp^2*xr-74*xr^2-120*xt;
char_series(I);

kill rng;

ring rng = 0,(xg, xa, xk, xh, xu),lp;
ideal I = -63*xa^2-4*xa+120,
116*xg*xa*xu-106*xg*xu-34*xa,
-7*xa*xu-111*xk^2,
-50*xg*xa*xu+86*xg*xh^2+56;
char_series(I);

kill rng;

ring rng = 0,(xw,xp, xy, xo ,xz),lp;
ideal I = 103*xw^2-61*xw*xy^2+43,
114*xp*xo-107*xp-107*xz,
92*xw+124*xy*xz^2;
char_series(I);

kill rng;

ring rng = 0,( xx, xz, xp, xo ,xn),lp;
ideal I = -61*xz*xn^2-10,
-89*xx*xz^2-53,
-22*xx*xz+71*xo,
9*xz*xp*xo+19*xp^2+119*xp*xn;
char_series(I);

kill rng;

ring rng = 0,(xt, xd ,xa ,xp, xe),lp;
ideal I = 32*xt^2*xd+51*xa^2,
135*xt*xa+62*xd*xa*xp+68*xd*xa*xe,
93*xt*xd+9*xd*xp*xe+103,
-106*xt*xd^2-125;
char_series(I);

kill rng;

ring rng = 0,(xv, xz,xx, xj, xf),dp;
ideal I = 1680700*xv^5*xz^11*xx^3-15253663446*xv^13*xx^6+204205050*xv^6*xz^8*xx^3+12301875*xz^13,
2*xz*xx+3*xv*xf,
-3361400*xv^4*xz^11*xx^4+30507326892*xv^12*xx^7-408410100*xv^5*xz^8*xx^4+36905625*xz^12*xf,
100842*xv^6*xx^3*xj+3430*xv^3*xz^4*xx+138915*xv^4*xz*xx+18225*xz^5*xj,
980*xv*xz^7*xx^3*xj+302526*xv^6*xx^4+39690*xv^2*xz^4*xx^3*xj+2025*xz^7*xf,
490*xv^2*xz^7*xx^2*xj+151263*xv^7*xx^3+19845*xv^3*xz^4*xx^2*xj-675*xz^8,
34300*xv^3*xz^11*xx-311299254*xv^11*xx^4+4167450*xv^4*xz^8*xx+182250*xz^12*xj+56260575*xv^5*xz^5*xx+7381125*xv*xz^9*xj,
-1960*xz^7*xx^4*xj-605052*xv^5*xx^5-79380*xv*xz^4*xx^4*xj+6075*xz^6*xf^2,
-201684*xv^5*xx^4*xj-6860*xv^2*xz^4*xx^2-277830*xv^3*xz*xx^2+54675*xz^4*xj*xf,
1029*xv^4*xx^2*xj+135*xz^4*xx*xj^2+35*xv*xz^4,
196*xv*xz^3*xx^3*xj^2-2058*xv^3*xx^2+405*xz^3*xj*xf,
98*xv^2*xz^3*xx^2*xj^2-1029*xv^4*xx-135*xz^4*xj,
3430*xv^3*xz^7*xx*xj+1058841*xv^8*xx^2+277830*xv^4*xz^4*xx*xj+18225*xz^8*xj^2,
57624*xv^3*xx^6*xj^2+1960*xz^4*xx^4*xj-6075*xz^3*xf^2,
28812*xv^4*xx^5*xj^2+980*xv*xz^4*xx^3*xj+2025*xz^4*xf,
14406*xv^5*xx^4*xj^2+490*xv^2*xz^4*xx^2*xj-675*xz^5,
38416*xv^2*xx^7*xj^2-1960*xz^3*xx^4*xj*xf+6075*xz^2*xf^3,
-392*xz^3*xx^4*xj^2+4116*xv^2*xx^3+1215*xz^2*xj*xf^2,
-2058*xv^3*xx^2*xj+405*xz^3*xj^2*xf-70*xz^4,
28*xx^4*xj^3-15*xf,
14*xv*xx^3*xj^3+5*xz,
686*xv^3*xz^3*xx*xj^2+27783*xv^4*xx*xj^2+3645*xz^4*xj^3-7203*xv^5,
-153664*xz^3*xx^8*xj^2-6223392*xv*xx^8*xj^2+1613472*xv^2*xx^7+1476225*xz*xf^4,
-392*xz^2*xx^4*xj^2*xf-2744*xv*xx^4+1215*xz*xj*xf^3,
54*xz^3*xx^2*xj^4+14*xv*xz^3*xx*xj^2-147*xv^3,
-153664*xz^2*xx^8*xj^2*xf+4148928*xx^9*xj^2-1075648*xv*xx^8+1476225*xf^5,
-1176*xz*xx^4*xj^2*xf^2+5488*xx^5+3645*xj*xf^4,
81*xz^2*xx*xj^4*xf-14*xz^3*xx*xj^2+147*xv^2,
81*xz*xj^4*xf^2-14*xz^2*xj^2*xf-98*xv,
243*xj^4*xf^3-42*xz*xj^2*xf^2+196*xx,
81*xx^3*xj^7*xf^2-14*xz*xx^3*xj^5*xf+35;
char_series(I);

//end J. Kroeker

tst_status(1);$
