LIB "tst.lib";
tst_init();

ring s=(0,m),(x,y,z,a,b,c),(dp(3),wp(4,6,14));
minpoly= m^7-1;
poly sq=m+m^4+m^2-m^6-m^3-m^5;
poly A=(m^5-m^2)/sq;

tst_status(1);$
