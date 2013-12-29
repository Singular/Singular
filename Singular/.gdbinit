break dErrorBreak
source gdb/helpers
source gdb/bos_gdb_ext.py

#source gdb/debug_FDeg
#source gdb/debug_order_of_pairs
#source gdb/debug_create_short
#source gdb/debug_everything
#source gdb/debug_initenterpairs
#source gdb/debug_redLP
#source gdb/debug_S
#source gdb/debug_L
#source gdb/debug_LS
#source gdb/print_set_sizes
#source gdb/watch_posInL

source gdb/print_sets
#print_sets__start_of_main_loop 0 0 1
print_sets__after_picking_pair 0 0 1

#set logging on
#set logging redirect on
#set logging overwrite on
#set logging file Watch_LSet.log

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

# source gdb/debug_redHomog
# source gdb/debug_FDeg
# blc ShiftDVec::bba 'red_result = strat->red(&strat->P,strat);'
