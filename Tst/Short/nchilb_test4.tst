LIB "tst.lib";
tst_init();
LIB"ncHilb.lib";

  ring r=0,U(1..3),dp;
  module p1=[1,U(2),U(3),U(3)];
  module p2=[1,U(2),U(2),U(3)];
  module p3=[1,U(1),U(3),U(3)];
  module p4=[1,U(1),U(3),U(2)];
  module p5=[1,U(1),U(2),U(3)];
  module p6=[1,U(1),U(2),U(2)];
  module p7=[1,U(1),U(1),U(3)];
  module p8=[1,U(1),U(1),U(2)];
  module p9=[1,U(2),U(3),U(2),U(3)];
  module p10=[1,U(2),U(3),U(1),U(3)];
  module p11=[1,U(2),U(3),U(1),U(2)];
  module p12=[1,U(1),U(3),U(1),U(3)];
  module p13=[1,U(1),U(3),U(1),U(2)];
  module p14=[1,U(1),U(2),U(1),U(3)];
  module p15=[1,U(1),U(2),U(1),U(2)];
  module p16=[1,U(2),U(3),U(2),U(1),U(3)];
  module p17=[1,U(2),U(3),U(2),U(1),U(2)];
  module p18=[1,U(2),U(3),U(2),U(2),U(1),U(3)];
  module p19=[1,U(2),U(3),U(2),U(2),U(1),U(2)];
  module p20=[1,U(2),U(3),U(2),U(2),U(2),U(1),U(3)];
  module p21=[1,U(2),U(3),U(2),U(2),U(2),U(1),U(2)];
  list ll=list(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,
  p14,p15,p16,p17,p18,p19,p20,p21);
  nchilb(ll,7,1);
tst_status(1);$
