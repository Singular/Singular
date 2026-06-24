LIB "tst.lib";
tst_init();

ring R = 0,(x,y,z),dp;
ideal I = x^2+2*y+1/3,x*y-z^3,5*x^3*y+7/11;
string s = string(I);

link l = "ssi2:w ssi2_options_plain.ssi2";
write(l,I);
close(l);
kill l;
link l = "ssi2:r ssi2_options_plain.ssi2";
def J = read(l);
close(l);
kill l;
s == string(J);
kill J;

link l = "ssi2:w ssi2_options_gzip.ssi2.gz";
write(l,I);
close(l);
kill l;
link l = "ssi2:r ssi2_options_gzip.ssi2.gz";
def G = read(l);
close(l);
kill l;
s == string(G);
kill G;

link l = "ssi2:w,plain ssi2_options_plain_suffix.ssi2.gz";
write(l,I);
close(l);
kill l;
link l = "ssi2:r,plain ssi2_options_plain_suffix.ssi2.gz";
def P = read(l);
close(l);
kill l;
s == string(P);
kill P;

link l = "ssi:w ssi2_options_legacy_named.ssi2";
write(l,I);
close(l);
kill l;
link l = "ssi:r ssi2_options_legacy_named.ssi2";
def O = read(l);
close(l);
kill l;
s == string(O);
kill O;

tst_status(1);$
