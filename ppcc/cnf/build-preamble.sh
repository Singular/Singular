#!/bin/sh
for CC in gcc clang cc false
do
  $CC -o cnf/genconfig cnf/genconfig.c && break
done

for CXX in g++ clang++ c++ false
do
  $CXX -o /dev/null cnf/genconfig.cc && break
done

jobs=0

yield() {
  jobs=`expr $jobs + 1`
  test $jobs -eq 5 && { wait; jobs=0; }
}
cc() {
  command $CC "$@" &
  yield
}
cxx() {
  command $CXX "$@" &
  yield
}
ar() {
  wait
  command ar "$@"
}
finish() {
  wait
}

cnf/genconfig >adlib/config.h
