break dErrorBreak
source gdb/helpers
#source gdb/bos_gdb_ext.py

set pagination off
#set logging overwrite on
#set logging redirect on
#set logging file log-file.txt
#set logging on
#source gdb/examine_reduction

# debug memory bugs considering L/TObjects
# set pagination off
# source gdb/debug_lt

# source gdb/dbg_strat_segfault

# debug messages for creation of S-Polynomials
# set pagination off
# source gdb/debug_spoly_creation

# debug messages for reduction of s-polynomials
# set pagination off
# source gdb/debug_spoly_reduction

# open(/dev/pts/11, 2, 0)
# call dup(0)
# call dup(0)
