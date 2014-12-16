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

kill rng;

ring r = 0,(xi,xs,xq,xx,xm),(dp(5),C);
ideal PS =
-172998863839362083328*xi^24*xs^12*xq^3*xx^2-52588753158715937280*xi^19*xs^13*xq^4*xx-3996524742145603200*xi^14*xs^14*xq^5-71016487260381000*xi^14*xs^12*xq^3*xx^3+214426195095580767936*xi^16*xs^8*xq^2*xx^3+10863661177099654560*xi^11*xs^9*xq^3*xx^2-88591320048324018744*xi^8*xs^4*xq*xx^4+252588087748157329728*xi^3*xs^7*xq^4+4488379247731300740*xi^3*xs^5*xq^2*xx^3+12200657953515011617*xx^5,24316515699137447759674331724288000*xi^51*xs^25*xq^4*xx+3695906474187557573535406813440000*xi^46*xs^26*xq^5+9981993459611016739859301000000*xi^41*xs^25*xq^4*xx^2-30139492385276843294717118219456000*xi^43*xs^21*xq^3*xx^2+20554637419790732695094892000000*xi^36*xs^26*xq^6*xm-1517179299416768791697692500000*xi^36*xs^26*xq^5*xx-20687462228947419066196365168384000*xi^38*xs^22*xq^5*xm+3053966733658380347822489267520000*xi^38*xs^22*xq^4*xx+232088666866071881778962758800000*xi^33*xs^23*xq^5+12452291170898620642854475864824000*xi^35*xs^17*xq^2*xx^3-2523525114996925217629488470160000*xi^30*xs^18*xq^3*xx^2-3897278432036671489948499945040000*xi^25*xs^19*xq^5*xm+287665991882511129368141374350000*xi^25*xs^19*xq^4*xx-289524899908583838376732433488291200*xi^27*xs^15*xq^3*xx-1714910052484122227705095818057000*xi^27*xs^13*xq*xx^4-29336904602896335824336969151504000*xi^22*xs^16*xq^4+260651947180500757879705339222500*xi^22*xs^14*xq^2*xx^3+199364580575358935943124077198858000*xi^19*xs^11*xq^2*xx^2+246315529136893170561682291829121600*xi^14*xs^12*xq^4*xm-18181046656243424975945277480399000*xi^14*xs^12*xq^3*xx+6099511078728236201969507465604580416*xi^16*xs^8*xq^2*xx+927074535026828353548824509971346080*xi^11*xs^9*xq^3-32947413642589064445136259421135300*xi^11*xs^7*xq*xx^3-5040090720940539781668577178927957328*xi^8*xs^4*xq*xx^2-5189205831027538000061138633925825888*xi^3*xs^5*xq^3*xm+383025762335628023240012416637606820*xi^3*xs^5*xq^2*xx+1041170109678996593607573204356058281*xx^3,87809640024663005798823975671040000*xi^46*xs^26*xq^5*xx+13346328934566180126655635715200000*xi^41*xs^27*xq^6+36046087493039782671714142500000*xi^36*xs^26*xq^5*xx^2-108837055835721934119811815792480000*xi^38*xs^22*xq^4*xx^2+74225079571466534732287110000000*xi^31*xs^27*xq^7*xm-5478703025671665081130556250000*xi^31*xs^27*xq^6*xx-74704724715643457739042429774720000*xi^33*xs^23*xq^6*xm+11028213204877484589358989021600000*xi^33*xs^23*xq^5*xx+5549738958397177241375463369691161600*xi^35*xs^19*xq^4*xx+838097963683037350868476629000000*xi^28*xs^24*xq^6+843513782984895911949100826630208000*xi^30*xs^20*xq^5+44966607006022796765863385067420000*xi^30*xs^18*xq^3*xx^3-6834547186450005797746531273350000*xi^25*xs^19*xq^4*xx^2-6878712277138422918674780919773539200*xi^27*xs^15*xq^3*xx^2-9382336966014209142468610978800000*xi^20*xs^20*xq^6*xm+692529239717156422552932938250000*xi^20*xs^20*xq^5*xx-4721482983124597979682098146116748800*xi^22*xs^16*xq^5*xm-348502194334406472120140892161832000*xi^22*xs^16*xq^4*xx-6192730745081552488935068231872500*xi^22*xs^14*xq^2*xx^4+350754228097287862950093735434296779264*xi^24*xs^12*xq^3*xx-52969411088562828571719527634660000*xi^17*xs^17*xq^5+941243142596252736787824836081250*xi^17*xs^15*xq^3*xx^3+53311701335541656318410473415066072320*xi^19*xs^13*xq^4+2841976469305289107547844603931306800*xi^19*xs^11*xq^2*xx^3+287971060831074018584512555953906000*xi^14*xs^12*xq^3*xx^2-434747910335560835498999032117402335168*xi^16*xs^8*xq^2*xx^2+296490914701815853453876832757276000*xi^9*xs^13*xq^5*xm-21884593197330048582156352522702500*xi^9*xs^13*xq^4*xx-298406849697781401880969749855731780352*xi^11*xs^9*xq^4*xm-391392551719721817287911771399348650*xi^11*xs^7*xq*xx^4-59488385743563588581496023954827625*xi^6*xs^8*xq^2*xx^3+179618405520415443598430155669725651672*xi^8*xs^4*xq*xx^3+123288373019930126966969810992237725408*xi^3*xs^5*xq^3*xx*xm-9100163801698196828012708795286589620*xi^3*xs^5*xq^2*xx^2-24736765709269953551573032337976695021*xx^4,-14433782545996547760000*xi^30*xs^18*xq^3*xx*xm-2193813908982703800000*xi^25*xs^19*xq^4*xm+161929835175610125000*xi^25*xs^19*xq^3*xx-325952463234374213328000*xi^27*xs^15*xq^2*xx-24771020528817746820000*xi^22*xs^16*xq^3-912242952938328692990400*xi^19*xs^11*xq^2*xx*xm+202003484075704335843000*xi^19*xs^11*xq*xx^2+138653279010542621052000*xi^14*xs^12*xq^3*xm-10234278543318353842500*xi^14*xs^12*xq^2*xx-10300412820786259198678560*xi^16*xs^8*xq*xx-27819603695730349764625*xi^11*xs^7*xx^3-57655517708797884729102816*xi^8*xs^4*xq*xx*xm+4255670201252115006646740*xi^8*xs^4*xx^2+98947452492997335990404712*xs^2*xq+23820683007573432738430764*xx^2*xm,-30806568*xi^8*xs^4*xq+15271632*xi^3*xs^3*xq^2*xm^2-1127230*xi^3*xs^3*xq*xx*xm+12727897*xx,9752*xi^3*xs*xq*xx*xm^3+2990*xi^3*xs^3*xq*xm-33761;
char_series(PS);

//end J. Kroeker

tst_status(1);$
