  ring r=0,(x,y,z),dp;
  ideal I=std(maxideal(2));
  attrib(I,"isSB");
  // although maxideal(2) is a standard basis,
  // Singular does not know it:
  attrib(maxideal(2), "isSB");
$
