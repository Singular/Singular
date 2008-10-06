//124. Beispiel mit Produktordnung, wo Macaulay ca 10 mal schneller ist:
// ca 43 sec auf PB 520c
// Macaulay hat 2 sec (ohne set autodegree, autocalc)
//option(mem);
//option(prot);

ring P6 = 31991,(s,t,x,y,a,b,c,d,e,f,g),dp;
ideal i= -6s5x+a, -5s4tx-1s5y+b, -4s3t2x-2s4ty+c,
-3s2t3x-3s3t2y+d, -2st4x-4s2t3y+e, -1t5x-5st4y+f, -6t5y+g;

;ideal j= eliminate(i,stxy);
j;

// andere Ringordnung:
ring Pa6 = 31991,(s,t,x,y,a,b,c,d,e,f,g),(a(1,1,1,1,6,6,6,6,6,6,6),dp(4),dp(7))
;
ideal i= -6s5x+a, -5s4tx-1s5y+b, -4s3t2x-2s4ty+c,
-3s2t3x-3s3t2y+d, -2st4x-4s2t3y+e, -1t5x-5st4y+f, -6t5y+g;
;ideal j=std(i);
LIB "elim.lib";
j=nselect(j,1..4);
j;
LIB "tst.lib";tst_status(1);$
==============
Macaulay input:
==============
ring R

11
stxya-g
1:4 6
4 6
ideal i
7
- -6s5x+a
- -5s4tx-1s5y+b
- -4s3t2x-2s4ty+c
- -3s2t3x-3s3t2y+d
- -2st4x-4s2t3y+e
- -1t5x-5st4y+f
- -6t5y+g
std i j
- -> 6[126k]7.8.9.10.11.12.13.14.15.16.17.18.19.20.21.22.23.24.25.26.27.
- -> 28.29.30.31.
- -> computation complete after degree 31
- -> elapsed time :  2 seconds


------- End of Forwarded Message


