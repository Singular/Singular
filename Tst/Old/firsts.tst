proc firsts
{
  string @ss = #[3] + #[3];
  string @s;
  int @i;
  for (int @j = 1; @j<=#[2]; @j=@j+1)
  {
    @s = "poly " + #[1] + string(@j) + "=";
    for (@i=1; @i <=size(#[3]); @i = @i + 1)
    {
      @s = @s + @ss[@i,@j] + "+";
    }
    @s = @s[1,size(@s)-1] + ";";
    @s;
  }
}
firsts("s",6,"abcdefg");
$;
