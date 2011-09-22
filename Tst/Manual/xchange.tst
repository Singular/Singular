LIB "tst.lib"; tst_init();
LIB "findifs.lib";
" EXAMPLE:"; 
ring r = (0,dt,dh,A),Tt,dp;
poly p = (Tt*dt+dh+1)^2+2*A;
string s = texpoly("",p);
s;
string t = xchange(s,"dh","dt");
t;
tst_status(1);$
