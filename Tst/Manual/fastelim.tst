LIB "tst.lib"; tst_init();
LIB "presolve.lib";
ring s=31991,(e,f,x,y,z,t,u,v,w,a,b,c,d),dp;
ideal i = w2+f2-1, x2+t2+a2-1,  y2+u2+b2-1, z2+v2+c2-1,
d2+e2-1, f4+2u, wa+tf, xy+tu+ab;
fastelim(i,xytua,1,1);       //with hilb,valvars
fastelim(i,xytua,1,0,1);     //with hilb,minbase
tst_status(1);$
