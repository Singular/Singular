LIB "tst.lib";
tst_init();

// elimpart nad matrix orderings
LIB"normal.lib";
intmat A[2][2]=1,0,11,6;
ring r3=3,(T(1..4),y,x),(dp(4), M(A));
ideal endid;
endid[1]=T(1)*y-T(1)-T(2)*x;
endid[2]=T(2)*y-T(3)*x;
endid[3]=-T(1)*x^2+T(3)*y-T(3)-T(4)*x^3;
endid[4]=T(4)*y-x^4;
endid[5]=-T(1)*x^2+y^2-y;
endid[6]=T(1)^2-T(3);
endid[7]=T(1)*T(2)-T(1)*x-T(4)*x^2;
endid[8]=T(2)^2-T(1)*y+T(1)+T(4)*x-x^5;
endid[9]=T(1)*T(3)-T(1)*y-x^5;
endid[10]=T(2)*T(3)-T(2)*y-y*x^4+x^4;
endid[11]=T(3)^2-T(3)*y-y^2*x^3+y*x^3;
endid[12]=T(1)*T(4)-y*x^2+x^2;
endid[13]=T(2)*T(4)-y^2*x-y*x-x;
endid[14]=T(3)*T(4)-y^3-y^2-y;
endid[15]=T(4)^2-T(2)*y+T(2)+y*x-x;
endid[16]=y^6-y^3*x^4-y^3+y^2*x^4-x^11;
list Le = elimpart(endid);
Le;

tst_status(1);$
