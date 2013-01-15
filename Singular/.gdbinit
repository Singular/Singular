break dErrorBreak

define pct
  call p_Write($arg0, currRing, strat->tailRing)
end

define ptt
  call p_Write($arg0, strat->tailRing, strat->tailRing)
end

set pagination off
#source gdbreak
#source dbgid1
source stop_print_bba_red_spoly

