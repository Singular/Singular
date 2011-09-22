LIB "tst.lib"; tst_init();
LIB "presolve.lib";
ring s   = 0,(e,f,x,y,z,t,u,v,w,a,b,c,d,H),dp;
ideal id = w2+f2-1, x2+t2+a2-1,  y2+u2+b2-1, z2+v2+c2-1,
d2+e2-1, f4+2u, wa+tf, xy+tu+ab;
id = homog(id,H);
hilbvec(id);
tst_status(1);$
