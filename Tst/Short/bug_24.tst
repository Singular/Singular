LIB "tst.lib";
tst_init();

ring r=0,(x,y,z),ds;
map m=r,x+y+z+y2+x3,x-y-z,z;
ideal id=2y+2z+x^3+y^2,x-y-z,y2+2z3;
id;
ideal pid=preimage(basering,m,id);
pid;
ideal imid=m(pid);
size(reduce(id,std(imid)));
size(reduce(imid,std(id)));

tst_status(1);$
