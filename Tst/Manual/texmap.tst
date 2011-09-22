LIB "tst.lib"; tst_init();
LIB "latex.lib";
// -------- prepare for example ---------
if (defined(TeXaligned)) {int Teali=TeXaligned; kill TeXaligned;}
if (defined(TeXreplace)) {list Terep=TeXreplace; kill TeXreplace;}
// -------- the example starts here ---------
//
string fname = "tldemo";
ring @r1=0,(x,y,z),dp;
export @r1;
ring r2=0,(u,v),dp;
map @phi =(@r1,u2,uv -v,v2); export @phi;
list TeXreplace;
TeXreplace[1] = list("@phi","\\phi");    // @phi --> \phi
export TeXreplace;
texmap("","@phi",@r1,r2);                // standard form
//
int TeXaligned; export TeXaligned;       // map in one line
texmap("",@phi,@r1,r2);
//
kill @r1,TeXreplace,TeXaligned;
//
// --- restore global variables if previously defined ---
if (defined(Teali)) {int TeXaligned=Teali; export TeXaligned; kill Teali;}
if (defined(Terep)) {list TeXreplace=Terep; export TeXreplace; kill Terep;}
tst_status(1);$
