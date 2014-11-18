LIB "tst.lib";
tst_init();

// resolve alias in imap/fetch
proc p1(alias ideal i)
{
    def br = basering;
    ring s = 0, x, ds;
    ideal i = fetch(br, i);
    setring(br);
    return(i);
}
proc p2(alias ideal i)
{
    def br = basering;
    ring s = 0, x, ds;
    ideal i = imap(br, i);
    setring(br);
    return(i);
}
ring r = 0, x, dp;
ideal i = x;
p1(i);
p2(i);

tst_status(1);$
