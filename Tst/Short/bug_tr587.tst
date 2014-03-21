LIB "tst.lib";
tst_init();

// sres crashes if qideal=ideal(0):

ring R_sv = 0,(x(1),x(2)),dp;
ideal ti_sv = 0;
qring r_sv = std(ti_sv);
ring R_s2v = 0,(z(1),z(2),z(3),z(4),z(5),z(6)),dp;
ideal ti_s2v = 1*z(6)^2-1*z(1)^1*z(3)^1,1*z(4)^1*z(5)^1-1*z(1)^1*z(3)^1,1*z(4)^1*z(6)^1-1*z(1)^1*z(5)^1,1*z(4)^2-1*z(1)^1*z(6)^1,1*z(5)^1*z(6)^1-1*z(3)^1*z(4)^1,1*z(5)^2-1*z(3)^1*z(6)^1;
qring r_s2v = std(ti_s2v);
ring R_sv_tensor_R_s2v = 0,(x(1),x(2),z(1),z(2),z(3),z(4),z(5),z(6)),dp;
ideal ti_sv = 0;
qring r_sv_tensor_R_s2v = std(ti_sv);
setring r_s2v;
map f = r_sv_tensor_R_s2v,1*z(2)^1,1*z(6)^1,z(1),z(2),z(3),z(4),z(5),z(6);
setring r_sv_tensor_R_s2v;
ideal sigma2 = kernel(r_s2v, f);
def L = res(sigma2,2);

tst_status(1);$
