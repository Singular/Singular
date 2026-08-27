LIB "tst.lib";
tst_init();

htable H = htable("a" -> 1, "b" -> 2);
typeof(H);
typeOfValue(H, "a");
parentOfValue(H, "a");
getValue(H, "a");
size(H);
H["a"];
H("b");

htable I = htable("c" -> 3);
htable J = H + I + ("d" -> 4);
size(J);
J["c"];
J["d"];

htable K = htable(key -> 7);
K["key"];

list pairs = list("e" -> 5, "f" -> intvec(1,2,3));
htable L = htable(pairs);
size(L);
L["e"];
L["f"];

apply(list(1,2), x->{x+1});

htable D = htable("a" -> 1, "a" -> 2);

ring r = 0, (x,y), dp;
poly p = x+y;
htable P = htable("p" -> p, "n" -> 3);
typeOfValue(P, "p");
typeof(parentOfValue(P, "p"));
varstr(parentOfValue(P, "p"));
typeOfValue(P, "n");
parentOfValue(P, "n");
typeof(basering);
P["p"];

ring s = 0, (a), dp;
setring s;
P["p"];
getValue(P, "p");
varstr(basering);
setring s;
getValue(P, "n");
varstr(basering);

setring r;
P["p"];

link hl = "ssi:w htable.ssi";
write(hl, P);
close(hl);
kill hl;

setring s;
link hl = "ssi:r htable.ssi";
def Q = read(hl);
close(hl);
kill hl;
typeof(Q);
size(Q);
typeOfValue(Q, "p");
typeof(parentOfValue(Q, "p"));
varstr(parentOfValue(Q, "p"));
typeOfValue(Q, "n");
parentOfValue(Q, "n");
varstr(basering);
getValue(Q, "p");
varstr(basering);
setring s;
getValue(Q, "n");
varstr(basering);

setring r;
number hn = 5/7;
poly hp = x+y;
vector hv = [x,y];
ideal hid = x,y;
matrix hmat[2][2] = x,1,0,y;
module hmod = [x,0],[0,y];
intvec hiv = intvec(1,2,3);
intmat him[2][2] = 1,2,3,4;
bigint hbig = bigint(2)^70;
bigintmat hbm[2][2] = hbig,2,3,4;
bigintvec hbv = hbig,2,3;
list hlist = list(1, "two", hiv);
proc hproc { return(17); }
ring hRing = 32003, u, dp;
setring r;
htable hNested = htable("inner" -> hp);
htable T = htable("int" -> 42, "string" -> "hello",
                  "bigint" -> hbig, "number" -> hn,
                  "poly" -> hp, "vector" -> hv, "ideal" -> hid,
                  "matrix" -> hmat, "module" -> hmod,
                  "intvec" -> hiv, "intmat" -> him,
                  "bigintmat" -> hbm, "bigintvec" -> hbv,
                  "list" -> hlist, "proc" -> hproc,
                  "ring" -> hRing,
                  "htable" -> hNested);
size(T);
typeOfValue(T, "int");
typeOfValue(T, "string");
typeOfValue(T, "bigint");
typeOfValue(T, "number");
typeOfValue(T, "poly");
typeOfValue(T, "vector");
typeOfValue(T, "ideal");
typeOfValue(T, "matrix");
typeOfValue(T, "module");
typeOfValue(T, "intvec");
typeOfValue(T, "intmat");
typeOfValue(T, "bigintmat");
typeOfValue(T, "bigintvec");
typeOfValue(T, "list");
typeOfValue(T, "proc");
typeOfValue(T, "ring");
typeOfValue(T, "htable");

link tl = "ssi:w htable_types.ssi";
write(tl, T);
close(tl);
kill tl;

setring hRing;
link tl = "ssi:r htable_types.ssi";
def U = read(tl);
close(tl);
kill tl;
size(U);
typeOfValue(U, "bigintmat");
typeOfValue(U, "proc");
varstr(basering);
getValue(U, "poly");
varstr(basering);
getValue(U, "number");
U["string"];
U["intvec"];
U["intmat"][2,1];
U["bigintvec"][1];
varstr(U["ring"]);
typeOfValue(getValue(U, "htable"), "inner");
getValue(getValue(U, "htable"), "inner");

tst_status(1);$
