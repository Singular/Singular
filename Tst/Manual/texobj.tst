LIB "tst.lib"; tst_init();
LIB "latex.lib";
// -------- prepare for example ---------
if (defined(TeXaligned)) {int Teali=TeXaligned; kill TeXaligned;}
if (defined(TeXbrack)){string Tebra=TeXbrack; kill TeXbrack;}
//
//  --------------  typesetting for polynomials ----------
ring r = 0,(x,y),lp;
poly f = x5y3 + 3xy4 + 2xy2 + y6;
f;
texobj("",f);
//  --------------  typesetting for ideals ----------
ideal G = jacob(f);
G;
texobj("",G);
//  --------------  variation of typesetting for ideals ----------
int TeXaligned = 1; export TeXaligned;
string TeXbrack = "<"; export TeXbrack;
texobj("",G);
kill TeXaligned, TeXbrack;
//  --------------  typesetting for matrices ----------
matrix J = jacob(G);
texobj("",J);
//  --------------  typesetting for intmats ----------
intmat m[3][4] = 9,2,4,5,2,5,-2,4,-6,10,-1,2,7;
texobj("",m);
//
// --- restore global variables if previously defined ---
if (defined(Teali)){int TeXaligned=Teali; export TeXaligned; kill Teali;}
if (defined(Tebra)){string TeXbrack=Tebra; export TeXbrack; kill Tebra;}
tst_status(1);$
