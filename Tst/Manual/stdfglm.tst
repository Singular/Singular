LIB "tst.lib"; tst_init();
   ring r  = 0,(x,y,z),lp;
ideal i = y3+x2,x2y+x2,x3-x2,z4-x2-y;
stdfglm(i);                   //uses fglm from "dp" (with groebner) to "lp"
stdfglm(i,"std");             //uses fglm from "dp" (with std) to "lp"
ring s  = (0,x),(y,z,u,v),lp;
minpoly = x2+1;
ideal i = u5-v4,zv-u2,zu3-v3,z2u-v2,z3-uv,yv-zu,yu-z2,yz-v,y2-u,u-xy2;
weight(i);
stdfglm(i,"(a(2,3,4,5),dp)"); //uses fglm from "(a(2,3,4,5),dp)" to "lp"
tst_status(1);$
