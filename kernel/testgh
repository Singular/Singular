#!/bin/sh
if [ -f grammar.hh ]
then
  mv grammar.hh grammar.cc.h
fi
if [ -f grammar.h ]
then
  if diff grammar.cc.h grammar.h >/dev/null
  then
    echo grammar.h was okay
    /bin/rm grammar.cc.h
  else
    echo generating new grammar.h
    mv grammar.cc.h grammar.h
  fi
else
  echo generating grammar.h
  mv grammar.cc.h grammar.h
fi
