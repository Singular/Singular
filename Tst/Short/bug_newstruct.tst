LIB "tst.lib";
tst_init();

LIB"customstd.so";
LIB"gfanlib.so";
// newstruct stuff
newstruct("pdivisor","list summands, cone tail");

list L;
cone c;
pdivisor P;
P.summands=L;
P.tail=c;
P;

// tr. #813
newstruct("s","string a");

tst_status(1);$
