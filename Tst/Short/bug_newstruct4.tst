LIB "tst.lib";
tst_init();

// shadow rings
newstruct("mystruct", "list l, def d, int i");
mystruct ms;
ms.i=2;
ms.r_l;
ring R;
ms.r_l;
ms.l = list(ideal(x),ideal(y),ideal(z));
ms.l;
ring S = 0,a,lp;
ms.d = ideal(a2+1);
ms.d;
ms;
def T = ms.r_l;
def U = ms.r_d;
ms.r_i;

tst_status(1);$
