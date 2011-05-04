int d;
int e;
int n;
int r;
int o;
int s;
int y;
int ode; int onr; int oeo; int osm;
for (d=2; d<9; d=d+1)
{
  for (e=2; e<9; e=e+1)
  {
    if (e == d)
    {
      e = e + 1;
      continue;
    }
    for (n=2; n<9; n=n+1)
    {
      if ( (n == d) or (n == e))
      {
        n = n + 1;
        continue;
      }
      for (r=2; r<9; r=r+1)
      {
        if ((r == d) or (r == e) or (r == n) )
        {
        r = r + 1;
        continue;
        }
        o = 0;
        s = 9;
        d, e, n, r, o, s;
        y = d + e;
        ode = y div 10;
        y = y % 10;
        if (!((n + r + ode) % 10 <> e ))
        {
          onr = (n + r + ode) div 10;
          if (!( (e + o + onr) % 10 <> n ))
          {
            oeo = (e + o + onr) div 10;
            if (!( (s + 1 + oeo) % 10 <> o ))
            {
              osm = (s + 1 + oeo) div 10;
              if (osm == 1)
              {
                " "+string(s )+string(e)+string(n)+string(d);
                " 1"+string(o)+string(r)+string(e);
                "1"+string(o)+string(n)+string(e)+string(y);
	LIB "tst.lib";tst_status(1);$
                      }
            }
          }
        }
      }
    }
  }
}
