LIB "tst.lib"; tst_init();
  proc milnor_number (poly p)
  {
    ideal i= std(jacob(p));
    int m_nr=vdim(i);
    if (m_nr<0)
    {
      "// not an isolated singularity";
    }
    return(m_nr);         // the value of m_nr is returned
  }
  ring r1=0,(x,y,z),ds;
  poly p=x^2+y^2+z^5;
  milnor_number(p);
tst_status(1);$
