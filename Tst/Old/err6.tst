// an error of 0.8.9e (pfister): febase crashed on buffer increases >4K
ring r=0,x,dp;
poly z=1024;
z=z^10; // 2^100
z=z^10; // 2^1000
z=z^10; // 2^10000
z=z^10; // 2^100000
z;
$
