if (1)
{
"if branch if(1)";
}
else
{
"else branch if(1)";
};
// ====================================================
if (1)
{
"if branch if(1) without else";
}
int i;
else
{
"else branch without if";
};
// ====================================================
if (0)
{
"if branch if(0)";
}
else
{
"else branch if(0)";
};
// ====================================================
if (0)
{
"if branch if(0) without else";
}
int j;
else
{
"else branch without if";
};
// ====================================================
if(0)
{
  if(0)
  {
     "wrong 0 0: if if";
  }
  else
  {
     "wrong 0 0: if else";
   };
}
else
{
  if(0)
  {
     "wrong 0 0: else if";
  }
  else
  {
     "nested if 0 0";
  };
};
// ====================================================
if(0)
{
  if(1)
  {
     "wrong 0 1: if else";
  }
  else
  {
     "wrong 0 1: if if";
   };
}
else
{
  if(1)
  {
     "nested if 0 1";
  }
  else
  {
     "wrong 0 1: else else";
   };
}
// ====================================================
if(1)
{
  if(0)
  {
     "wrong 1 0: if if";
  }
  else
  {
     "nested if 1 0";
  };
}
else
{
  if(0)
  {
     "wrong 1 0: else if";
  }
  else
  {
     "wrong 1 0: else else";
   };
}
// ====================================================
if(1)
{
  if(1)
  {
     "nested if 1 1";
  }
  else
  {
     "wrong 1 1: if else";
   };
}
else
{
  if(1)
  {
     "wrong 1 1: else if";
  }
  else
  {
     "wrong 1 1: else else";
   };
}
$
