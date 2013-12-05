LIB "tst.lib";
tst_init();

// charstr +execute: ring represenation
string S;

ring r0dp = (0,t),(x,y),dp;
string(basering);
charstr(basering);
S = "ring rnew = ("+charstr(basering)+"),("+ varstr(basering) +"),"+"lp;";
S;
execute(S);
basering;

// ----------------------------------------------------

ring rZ = integer,(x,y),dp;
string(basering);

charstr(basering);

S = "ring rnew = ("+charstr(basering)+"),("+ varstr(basering) +"),"+"lp;";
S;
execute(S);
basering;

// ----------------------------------------------------

ring rZ8 = (integer,8),(x,y),dp;
string(basering);

charstr(basering);
S = "ring rnew = ("+charstr(basering)+"),("+ varstr(basering) +"),"+"lp;";
S;

execute(S);
basering;

ringlist(basering);

ring r = (real,50),(x,y,z),dp;
string(basering);

ring r = (real,10,50),(x,y,z),dp;
string(basering);

ring cR1050 = (complex,10,50),(x,y),dp;
basering;

charstr(basering);
S = "ring cR1050new = ("+charstr(basering)+"),("+ varstr(basering) +"),"+"lp;";
S;
execute(S);

ring r6int = (integer,6,3),x,dp;
string(basering);
charstr(basering);

ring r5int = (integer,5,3),x,dp;
string(basering);
charstr(basering);

ring r2int = (integer,2,3),x,dp;
string(basering);
charstr(basering);


tst_status(1);$
