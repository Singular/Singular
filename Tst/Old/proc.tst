proc p1
{
  "in p1";
  p2();
  "again in p1";
  return ();
}
proc p2
{
  "in p2";
  listvar(all);
  listvar(proc);
  "again in p2";
  return ();
}
proc rec
{
  "in rec",#[1];
  if (#[1] >0)
  {
    rec(#[1]-1);
  }
  if (#[1]==0)
  {
    listvar(all);
    listvar(proc);
  }
  "again in rec",#[1];
  return ();
}
"call p1";
p1();
"call p2";
p2();
"call rec(3)";
rec(3);
"end";
$
