proc tt
{
  "echo inside a proc",echo;
  "(voice is:",voice,")";
  return(0);
}
"t1:echo=",echo;
tt();
<"echo1.tst";
"t1:echo=",echo, voice;
$
