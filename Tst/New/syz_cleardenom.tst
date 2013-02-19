LIB "tst.lib"; tst_init();
ring R=(0,a),(x),dp;
minpoly = a2+1;
ideal point = (1/33424*a), (-531/474290437184*a+1/113520928), 0;
syz(point);
tst_status(1);$
