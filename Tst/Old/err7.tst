proc t
{
ring r;
listvar();
"returning";
}
t();
listvar();
kill t;
// error of 0.9.3a (pfister): mapping of zero mons
ring r1=0,s,dp;
poly te=s+3;
ring r=3,s,dp;
poly te=imap(r1,te);
te;
kill r;
// error of 0.9.3a (pfister): power of binoms in char p
ring r=3,(s,t),dp;
(s+t)^13;
poly p=(s+t)^13;p;
p=p*1;
p;
kill r;
// error of 0.9.3b (pfister): incorrect impl. for finding alg*pp
ring rq=(32003,p,q,s),(t,u,v,w,x,y,z),dp;
ideal ser;
kill rq;
// error of 0.9.3b (hannes): 0-monomial
ring r=0,(x,y),dp;
ideal i=ideal(0,x);
size(i[1]);// must be 0, was 1
LIB "tst.lib";tst_status(1);$
LIB "tst.lib";tst_status(1);$
