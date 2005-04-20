   LIB "hnoether.lib";
   ring r=0,(x,y),dp;
"0,2 <Return> 1,1 <Return> 3,0 <Return><Return>";
   hnexpansion(x2-y2+x3);
LIB "tst.lib";tst_status(1);$
//
//Der gleiche Fehler tritt auf bei reddevelop((y-x)*(y-2x)*(y-3x+x2));
//  (Newtonpolygon 0,3  2,1  4,0 eingeben),
//er tritt aber nicht auf z.B. bei 
// reddevelop((y2-x3)*(y2-x3-x4));     (nichts mehr einzugeben)  oder
// reddevelop((y2-x3-x4)*(y2+x3+x4));  ( 0,2  2,1  6,0 eingeben)
//
//(Singular92j produziert uebrigens das gleiche Ergebnis wie Singular93b)
