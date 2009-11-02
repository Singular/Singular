// $Id$

//
// finduni_s.tst - short tests for finduni
//


LIB "tst.lib";
tst_init();
tst_ignore("CVS: $Id$");

option(redSB);

// ======================
// cyclic 6 in char 32003
ring r=32003,(a,b,c,d,x,f), dp;
ideal i=a+b+c+d+x+f, ab+bc+cd+dx+xf+af, abc+bcd+cdx+d*xf+axf+abf, abcd+bcdx+cd*xf+ad*xf+abxf+abcf, abcdx+bcd*xf+acd*xf+abd*xf+abcxf+abcdf, abcd*xf-1;
ideal is=std(i);
finduni(is);
kill r;

// =================
// walks-7 in char 0
ring r=0,(a,b,c,d,e,f,g), dp;
ideal i=2gb+2fc+2ed+a2+a, 2gc+2fd+e2+2ba+b, 2gd+2fe+2ca+c+b2, 2ge+f2+2da+d+2cb, 2gf+2ea+e+2db+c2, g2+2fa+f+2eb+2dc, 2ga+g+2fb+2ec+d2;
ideal is=std(i);
finduni(is);
kill r;

tst_status(1);$
