LIB "tst.lib"; tst_init();
LIB "resolve.lib";
ring R=0,(x,y,z),dp;
ideal J=x3+y5+yz2+xy4;
list L=resolve(J,0);
def Q=L[1][7];
setring Q;
showBO(BO);
tst_status(1);$
