ring cyc5 =32003,(f,a,b,c,d,e),dp;
option(prot);
ideal i=
a+b+c+d+e,
de+1cd+1bc+1ae+1ab,
cde+1bcd+1ade+1abe+1abc,
bcde+1acde+1abde+1abce+1abcd,
abcde-f5;
// should only complain, but not dump core as in 1.3.6
list I =sres(i,0);
sres(std(i), 0);
LIB "tst.lib"; tst_status(1);
$
