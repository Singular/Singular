#! /bin/sh
# This script emulates some programs so that make does not fail
# 

echo "***Warning: Program '$1' not found. Make might be incomplete or fail."

case $1 in
  gzip) 
    echo "cp $2 $2.gz"; 
    cp $2 $2.gz;;
  gunzip) 
    filename=`echo $2 | sed -e "s/\.gz//"`; 
    echo  "cp $2 $filename"; 
    cp $2 $filename;;
  makeinfo) 
    dest=`echo $* | sed -e 's/.*-o *\([^ ]*\).*/\1/'`; 
    src=`echo $* | sed -e 's/.* \([^ ]*\)/\1/'`; 
    echo "cp $src $dest";
    cp $src $dest;;
esac

