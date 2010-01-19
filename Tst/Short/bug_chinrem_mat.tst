LIB "tst.lib";
tst_init();

ring R = 0, (x,y,z), dp;
matrix m1[3][2] = 3,4,5,6,7,1;
matrix m2[3][2] = 8,2,667,64,7,4;
matrix m3[3][2] = z,y,x,64,7,1;
list T1 = m1,m2,m3;
matrix trans = chinrem(T1,intvec(3,7,13));
trans;
print(trans);

tst_status(1);$
