//
// test script for string command
//
proc last
{
  string @s = #[1];
  int    @i = size(@s);
  return (@s[@i,1]);
}
proc pos
{
  int @start;
  if (size(#) == 2)
  {
    @start = 1;
  }
  if (size(#) == 3)
  {
    @start = #[3];
  }
  string @s = #[1];
  int    @l = size(#[2]);
  //"start:",@start;
  //"end<",size(@s);
  //"arg1:",#[1];
  //"arg2:",#[2];
  //"arg:",@s;
  //"len:",@l;
  for (int @i=@start; @i<size(@s); @i=@i+1)
  {
    //"test >>"+@s[@i,@l]+"<< und >>"+#[2]+"<<";
    if (@s[@i,@l] == #[2])
    {
      return(@i);
    }
  }
  return (0);
}
proc max
{
  if (#[1] < #[2])
  {
    return (#[2]);
  }
  return (#[1]);
}
"string sss = \"dum di dum\"";string sss = "dum di dum";
"pos(sss,\"di d\")", pos(sss,"di d");
"sss",sss;
"\"dum di dum\"[2,6]","dum di dum"[2,6];
"size(sss)", size(sss);
"sss[10,1]", sss[10,1];
"last(sss)", last(sss);
"sss[1,size(sss)-1]",sss[1,size(sss)-1];
"size(\"dddd sss\")",size("dddd sss");

string eee =
"1234567890",
"1234567890",
"12345+7890",
"1234567890",
"123456+890",
"1234567890",
"1234+67890",
"12345+7890",
"1234567890",
"123456+890",
"1234567890",
"1234+67890",
"12345+7890",
"1234567890",
"123456+890",
"1234567890",
"1234+67890",
"12345+7890",
"1234567890",
"123456+890",
"1234567890",
"1234A67890",
"12345A7890",
"1234567890",
"123456A890",
"1234567890",
"1234A67890",
"12345A7890",
"1234567890",
"123456A890",
"1234567890",
"1234A67890",
"1234567+90",
"1234567890";
eee;
proc split
{
  string @s = #[1];
  int @ipos = pos(@s,"+");
  if ((@ipos <= 1) || (@ipos > 60))
  {
    @s;
    return("");
  }
  if (size(@s) < 60)
  {
    @s;
    return("");
  }

  int @i = 0;
  int @j = 0;
  while (1)
  {
    @i = pos(@s,"+",@j+1);
    if ((@i>60) || (@i==0))
    {
      @s[1,@j], "--MM";
      @s = split(@s[@j+1,size(@s)-@j]);
      return("");
    }
    @j = @i;
  }

  return("");
}
eee = ".......",split(eee);
"--------------------------------";
string s1="He said:";
string s2="\"It is true!\" ";
string s3="\"It is false!\" ";
string s4=s1+" "+s2;
s4;
"------------------------------";
string s5=s1+" "+s3;
s5;
"------------------------------";
s1,s2;
"------------------------------";
s1[0];
s1[1];
s1[5];
s1[8];
s1[9];
s1[0]=".";
s1[1]=".";
s1[5]=".";
s1[8]=".";
s1[9]=".";

listvar(all);
$
