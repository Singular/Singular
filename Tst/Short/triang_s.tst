LIB "tst.lib";
tst_init();

LIB "triang.lib";

ring rC5 = (0),(e,d,c,b,a),(c,lp(5));
ideal i = e+d+c+b+a,
ed+ea+dc+cb+ba,
edc+eda+eba+dcb+cba,
edcb+edca+edba+ecba+dcba,
edcba-1;
ideal G = a15+122a10-122a5-1,
55b2a5-55b2-2ba11-231ba6+233ba-8a12-979a7+987a2,
55b7+165b6a+55b5a2-55b2-398ba11-48554ba6+48787ba-1042a12-127116a7+128103a2,
55ca5-55c+a11+143a6-144a,
275cb-275ca+440b6a+1210b5a2-275b3a4+275b2-442ba11-53911ba6+53913ba-1121a12-136763a7+136674a2,
275c3+550c2a-550ca2+275b6a2+550b5a3-550b4a4+550b2a-232ba12-28336ba7+28018ba2-568a13-69289a8+69307a3,
55da5-55d+a11+143a6-144a,
275db-275da-110b6a-440b5a2-275b4a3+275b3a4+124ba11+15092ba6-15106ba+346a12+42218a7-42124a2,
275dc-275da+275c2+550ca-330b6a-1045b5a2-275b4a3+275b3a4-550b2+334ba11+40722ba6-40726ba+867a12+105776a7-105873a2,
275d2+825da+550b6a+1650b5a2+275b4a3-550b3a4+275b2-566ba11-69003ba6+69019ba-1467a12-178981a7+179073a2,
e+d+c+b+a;
attrib(G,"isSB",1);
triangL(G);
triangLfak(G);
triangM(G);
triangM(G, 2);
triangMH(G);
triangMH(G,2);

tst_status(1);$
