LIB "tst.lib";
tst_init();

LIB"gfanlib.so";
// newstruct stuff
newstruct("pdivisor","list summands, cone tail");

list L;
cone c;
pdivisor P;
P.summands=L;
P.tail=c;
P;

tst_status(1);$
