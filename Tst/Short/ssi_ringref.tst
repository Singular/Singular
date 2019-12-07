LIB "tst.lib";
tst_init();

ring R = 0,x,dp;
ideal I = x;
link l = "ssi:w ideal1.ssi";
write(l,I);
close(l);
kill I;
kill l;
ring S = 0,y,dp;
link l = "ssi:w ideal2.ssi";
ideal I = y;
write(l,I);
close(l);
kill I;
kill l;
kill R;
// -----------------------------
link l = "ssi:r ideal1.ssi"; def I = read(l); close(l); kill l;
I;
kill I;
link l = "ssi:r ideal2.ssi"; def I = read(l); close(l); kill l;
I;
kill I;
link l = "ssi:r ideal1.ssi"; def I = read(l); close(l); kill l;
I;
kill I;
link l = "ssi:r ideal2.ssi"; def I = read(l); close(l); kill l;
I;
kill I;
listvar();
// -----------------------------
tst_status(1);$

