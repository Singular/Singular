import gdb
import re
import shlex

"""
return a line number which contains a string (as substring) 
This function takes three arguments: the function in which to
search for the specific line, the string to search for and
the maxium number of lines to scan, which defaults to 1000.
If substring not found: return -1
"""
def lnr(function_name, line_contains, max_scan_lines = 1000):
  listsize_sav = gdb.execute("show listsize", False, True)
  m = re.search('[0-9]+', listsize_sav)
  listsize_sav = m.group(0)

  gdb.execute("set listsize " + str(max_scan_lines), False, True)
  lines_str = gdb.execute\
      ("list " + function_name + ",", False, True)
  lines = lines_str.split("\n")
  lnr = -1
  for line in lines:
    if line_contains in line:
      m = re.search('^[0-9]+', line)
      lnr = m.group(0)
      break

  gdb.execute("set listsize " + str(listsize_sav), False, True)
  return lnr

# get the filename a function is contained in (first match)
def fn(function_name):
  res_str = gdb.execute("info func " + function_name,False, True)
  res_lns = res_str.split("\n")
  return res_lns[2][5:-1]

def brs(function_name, line_contains, max_scan_lines = 1000):
  return fn(function_name) + ":"\
      + str(lnr(function_name, line_contains, max_scan_lines))

#class BreakLineContains(gdb.Breakpoint):
#  def __init__\
#      (self,function_name,line_contains,max_scan_lines):
#    break_string =\
#        brs(function_name,line_contains,max_scan_lines)
#    super(BreakLineContains, self)\
#        .__init__(break_string,gdb.BP_BREAKPOINT, internal=False)

"""
SYNOPSIS:
  blc <function> <string>
DESCRIPTION:
  Will set a breakpoint to a specific line in the function
  specified by <function>. The line will be the first line found,
  which contains the string <string>.
BUGS:
- If the function does not contain a line containing <string>,
  then a breakpoint might be set to a line after the function
  <function> in the same file.
- Will only scan first 1000 lines of a function.
- If more than one function with the name <function> exist, only
  the first one found will be considered.
"""
class BreakLineContainsCmd(gdb.Command):
    def __init__(self):
      super(BreakLineContainsCmd, self).__init__\
          ('blc', gdb.COMMAND_DATA, gdb.COMPLETE_SYMBOL)

    def invoke(self, args, from_tty):
      args = shlex.split(args)
      if len(args) == 0:
        raise gdb.GdbError('function name missing')
      if len(args) == 1:
        raise gdb.GdbError('search string missing')
      if lnr(args[0], args[1]) < 0:
        raise gdb.GdbError('could not find matching line!')
      break_at = brs(args[0], args[1])
      print "setting breakpoint to " + break_at
      gdb.execute("b " + break_at)
      #BreakLineContains\
      #    (function_name, line_contains, max_scan_lines)

BreakLineContainsCmd()

#    def stop(self):
#        gdb.execute(self.command)
#        return False
#class GetLineNr (gdb.Function):
#  """
#  return a line number which contains a string (as substring) 
#  This function takes three arguments: the function in which to
#  search for the specific line, the string to search for and
#  the maxium number of lines to scan, which defaults to 1000.
#  If substring not found: return -1
#  """
#  def __init__ (self):
#    super (GetLineNr, self).__init__ ("lnr")
#
#  def invoke (self, fname, line_contains, max_scan_lines = 1000):
#    print fname.string()
#    listsize_sav = gdb.execute("show listsize", False, True)
#    m = re.search('[0-9]+', listsize_sav)
#    listsize_sav = m.group(0)
#
#    gdb.execute\
#        ("set listsize " + str(max_scan_lines), False, True)
#    #listcmd = ' '.join(("hello", str(fname)))
#    #print listcmd
#    #print line_contains
#    #lines_str = gdb.execute(listcmd, False, True)
#    #lines = lines_str.split("\n")
#    #for line in lines:
#    #  if line_contains in line:
#    #    m = re.search('^[0-9]+')
#    #    lnr = m.group(0)
#    #    print lnr
#    #    return lnr
#
#    ## print lines[0]
#    #gdb.execute\
#    #    ("set listsize " + str(listsize_sav), False, True)
#    return -1
#
#GetLineNr()
#class GetLineNr (gdb.Function):
#  """
#  return a line number which contains regular expression
#  This function takes two arguments: the function in which to
#  search for the specific line and the regular expression.
#  """
#
#  def __init__ (self):
#    super (CallerIs, self).__init__ ("caller_is")
#
#  def invoke (self, name, regex):
#    #lines_str = gdb.execute("list ShiftDVec::bba", False, True)
#    print "hello"
#    #print lines_str
#    #lines = lines_str.split("\n")
#    #print line[0]
#    #return line[0]
#    return 1
#
#GetLineNr()

# b ShiftDVec::bba
# call $caller_is("hello","")
# source gdb/gdb_get_line.py
# LIB "freegb.lib";
# ring r = 5,(x,y),dp;
# def R = makeLetterplaceRing(10);
# setring R;
# option(prot); option(redTail);
# option(redSB); option(notBuckets);
# ideal I = x(1)*x(2)+4, y(1)*x(2)*y(3)+3*x(1),y(1)*y(2)*y(3)+2;
# ideal J = system("freegbdvc",I,10,2);
