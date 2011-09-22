LIB "tst.lib"; tst_init();
LIB "mprimdec.lib";
ring s1=(0,x,y),(a,b,c,d,e,f,g),lp;
ideal i=ea-fbg,fa+be,ec-fdg,fc+de;
i=std(i);
list  l=indSet(i);
l;
tst_status(1);$
