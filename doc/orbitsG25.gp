G := Group(
 (2, 3)(5, 6)(9, 10), (1, 5, 9, 10, 3)(2, 7, 8, 4, 6)
);;
Size(G);
dimQ:=5;

minidx:=SmallestMovedPoint(G);
maxidx:=LargestMovedPoint(G);
XZorbitsRepresentatives:=[];;
for k in [dimQ..(maxidx-minidx+1)] do
  Print("Considering faces of cardinality ",k,"\n");
  XZ := Combinations([minidx..maxidx],k);;
  Bahnen := OrbitsDomain(G,XZ,OnSets);;
  for i in [1..Size(Bahnen)] do
    Append(XZorbitsRepresentatives,[Bahnen[i][1]]);
  od;
od;

PrintTo("simplexOrbitRepresentativesG25.sing","list simplexOrbitRepresentatives = ");
for k in [1..Size(XZorbitsRepresentatives)-1] do
  s:=String(XZorbitsRepresentatives[k]);
  s:=s{[2..Size(s)-1]};
  AppendTo ("simplexOrbitRepresentativesG25.sing","intvec(",s,"),\n");
od;
s:=String(XZorbitsRepresentatives[Size(XZorbitsRepresentatives)]);
s:=s{[2..Size(s)-1]};
AppendTo ("simplexOrbitRepresentativesG25.sing","intvec(",s,");\n");


PrintTo("simplexSymmetryGroupG25.sing","list simplexSymmetryGroup = ");
elementsG:=Elements(G);
for i in [1..Size(elementsG)-1] do
   sigma:=elementsG[i];
   l:=ListPerm(sigma,maxidx);
   l:=l{[minidx..maxidx]};
   s:=String(l);
   s:=s{[2..Size(s)-1]};
   AppendTo ("simplexSymmetryGroupG25.sing","permutationFromIntvec(intvec(",s,")),\n");
od;
sigma:=elementsG[Size(elementsG)];
l:=ListPerm(sigma,maxidx);
l:=l{[minidx..maxidx]};
s:=String(l);
s:=s{[2..Size(s)-1]};
AppendTo ("simplexSymmetryGroupG25.sing","permutationFromIntvec(intvec(",s,"));\n");


PrintTo("elementsInTermsOfGeneratorsG25.sing","list generatorsG = ");
L:=GeneratorsOfGroup(G);
for i in [1..Size(L)-1] do
   sigma:=L[i];
   l:=ListPerm(sigma,maxidx);
   l:=l{[minidx..maxidx]};
   s:=String(l);
   s:=s{[2..Size(s)-1]};
   AppendTo ("elementsInTermsOfGeneratorsG25.sing","permutationFromIntvec(intvec(",s,")),\n");
od;
   sigma:=L[Size(L)];
   l:=ListPerm(sigma,maxidx);
   l:=l{[minidx..maxidx]};
   s:=String(l);
   s:=s{[2..Size(s)-1]};
   AppendTo ("elementsInTermsOfGeneratorsG25.sing","permutationFromIntvec(intvec(",s,"));\n");

AppendTo("elementsInTermsOfGeneratorsG25.sing","list elementsInTermsOfGenerators = ");
hom:=EpimorphismFromFreeGroup(G);
   AppendTo ("elementsInTermsOfGeneratorsG25.sing","\"\",\n");
for i in [2..Size(elementsG)-1] do
   sigma:=elementsG[i];
   l:=PreImagesRepresentative(hom,sigma);
   s:=String(l);
   AppendTo ("elementsInTermsOfGeneratorsG25.sing","\"",s,"\",\n");
od;
sigma:=elementsG[Size(elementsG)];
l:=PreImagesRepresentative(hom,sigma);
s:=String(l);
AppendTo ("elementsInTermsOfGeneratorsG25.sing","\"",s,"\";\n");


