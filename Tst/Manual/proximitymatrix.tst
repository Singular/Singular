LIB "tst.lib"; tst_init();
LIB "alexpoly.lib";
ring r=0,(x,y),ls;
poly f1=(y2-x3)^2-4x5y-x7;
poly f2=y2-x3;
poly f3=y3-x2;
list proximity=proximitymatrix(f1*f2*f3);
/// The proximity matrix P ///
print(proximity[1]);
/// The proximity resolution graph N ///
print(proximity[2]);
/// They satisfy N=-transpose(P)*P ///
print(-transpose(proximity[1])*proximity[1]);
/// The incidence matrix of the Enriques diagram ///
print(proximity[3]);
/// If M is the matrix of multiplicities and TM the matrix of total
/// multiplicities of the singularity, then  M=P*TM.
/// We therefore calculate the (total) multiplicities. Note that
/// they have to be slightly extended.
list MULT=extend_multiplicities(totalmultiplicities(f1*f2*f3));
intmat TM=MULT[1];  // Total multiplicites.
intmat M=MULT[2];   // Multiplicities.
/// Check: M-P*TM=0.
M-proximity[1]*TM;
/// Check: inverse(P)*M-TM=0.
intmat_inverse(proximity[1])*M-TM;
tst_status(1);$
