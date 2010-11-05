LIB "tst.lib";
tst_init();

// execute in realrad.lib

LIB "realrad.lib";
ring RR=0,(v,q,u,p),lp;
ideal i=v2-4vq+q2+1;
realrad(i);

ring R=0,(v,q,u,p),lp;
ideal i=v2-4vq+q2+1;
realrad(i);

ring Rg=0,(v,g,u,p),lp;
ideal i=v2-4vg+g2+1;
realrad(i);

ring Rf=0,(f,q,u,p),lp;
ideal i=f2-4fq+q2+1;
realrad(i);

tst_status(1);$
