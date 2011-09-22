LIB "tst.lib"; tst_init();
LIB "findifs.lib";
" EXAMPLE:"; 
ring r = (0,dt,theta),Tt,dp;
poly p = (Tt*dt+theta+1)^2+2;
string s = texfactorize("",p);
s;
s = replace(s,"Tt","T_t"); s;
s = replace(s,"dt","\\tri t"); s;
s = replace(s,"theta","\\theta"); s;
tst_status(1);$
