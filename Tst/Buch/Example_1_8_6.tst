LIB "tst.lib";
tst_init();

ring A =0,(u,v,x,y,z),dp;
ideal I=x-uv,y-uv2,z-u2;
ideal J=eliminate(I,uv);
J;

tst_status(1);$
