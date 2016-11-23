G := Group(
 (17,21) (18,22) (19,23) (20,24) (35,40) (36,39) (37,38),
 (16,17) (22,25) (23,26) (24,27) (32,35) (33,36) (34,37),
 (17,18) (21,22) (26,28) (27,29) (31,32) (36,38) (37,39),
 (18,19) (22,23) (25,26) (29,30) (32,33) (35,36) (39,40),
 (19,20) (23,24) (26,27) (28,29) (33,34) (36,37) (38,39)
);;
Size(G);
dimQ:=16;
minidx:=SmallestMovedPoint(G);
maxidx:=LargestMovedPoint(G);



XZorbitsRepresentatives:=[];;
for k in [dimQ..(maxidx-minidx)] do
  Print("Considering faces of cardinality ",k,"\n");
  XZ := Combinations([minidx..maxidx],k);;
  Print(Size(XZ));
  Bahnen := OrbitsDomain(G,XZ,OnSets);;
  for i in [1..Size(Bahnen)] do
    Print(i);
    Append(XZorbitsRepresentatives,[Bahnen[i][1]]);
  od;
od;

PrintTo("simplexOrbitRepresentatives.sing","list simplexOrbitRepresentatives = ");
for k in [1..Size(XZorbitsRepresentatives)-1] do
  s:=String(XZorbitsRepresentatives[k]);
  s:=s{[2..Size(s)-1]};
  AppendTo ("simplexOrbitRepresentatives.sing","intvec(",s,"),\n");
od;
s:=String(XZorbitsRepresentatives[Size(XZorbitsRepresentatives)]);
s:=s{[2..Size(s)-1]};
AppendTo ("simplexOrbitRepresentatives.sing","intvec(",s,");\n");


PrintTo("simplexSymmetryGroup.sing","list simplexSymmetryGroup = ");
elementsG:=Elements(G);
for i in [1..Size(elementsG)-1] do
   sigma:=elementsG[i];
   l:=ListPerm(sigma,maxidx);
   l:=l{[minidx..maxidx]};
   s:=String(l);
   s:=s{[2..Size(s)-1]};
   AppendTo ("simplexSymmetryGroup.sing","permutationFromIntvec(intvec(",s,")),\n");
od;
sigma:=elementsG[Size(elementsG)];
l:=ListPerm(sigma,maxidx);
l:=l{[minidx..maxidx]};
s:=String(l);
s:=s{[2..Size(s)-1]};
AppendTo ("simplexSymmetryGroup.sing","permutationFromIntvec(intvec(",s,"));\n");


PrintTo("elementsInTermsOfGenerators.sing","list generatorsG = ");
L:=GeneratorsOfGroup(G);
for i in [1..Size(L)-1] do
   sigma:=L[i];
   l:=ListPerm(sigma,maxidx);
   l:=l{[minidx..maxidx]};
   s:=String(l);
   s:=s{[2..Size(s)-1]};
   AppendTo ("elementsInTermsOfGenerators.sing","permutationFromIntvec(intvec(",s,")),\n");
od;
   sigma:=L[Size(L)];
   l:=ListPerm(sigma,maxidx);
   l:=l{[minidx..maxidx]};
   s:=String(l);
   s:=s{[2..Size(s)-1]};
   AppendTo ("elementsInTermsOfGenerators.sing","permutationFromIntvec(intvec(",s,"));\n");

AppendTo("elementsInTermsOfGenerators.sing","list elementsInTermsOfGenerators = ");
hom:=EpimorphismFromFreeGroup(G);
   AppendTo ("elementsInTermsOfGenerators.sing","\"\",\n");
for i in [2..Size(elementsG)-1] do
   sigma:=elementsG[i];
   l:=PreImagesRepresentative(hom,sigma);
   s:=String(l);
   AppendTo ("elementsInTermsOfGenerators.sing","\"",s,"\",\n");
od;
sigma:=elementsG[Size(elementsG)];
l:=PreImagesRepresentative(hom,sigma);
s:=String(l);
AppendTo ("elementsInTermsOfGenerators.sing","\"",s,"\";\n");


