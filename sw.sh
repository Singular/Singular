#!/bin/sh

PWD=`pwd`
BRANCH="spielwiese"
CFGOPTIONS="--with-gmp=yes"
MKOPTIONS="-j9"
# tempdir (absolute path!)
SW="$PWD/SW"

# we try to use log instead of consequent --quiet
LOG="$PWD/`date`.log"

# log or no log?
echo >> $LOG || LOG="/dev/null"

echo "DATE: `date`" >> $LOG

CleanUp() 
{
  echo "Deleting tempdir: $SW" >> $LOG
  rm -f -R $SW 1>> $LOG 2>&1
}

Build()
{
  echo "Generating configure... " >> $LOG
  ./for_Hans_with_love.sh 1>> $LOG 2>&1  || { echo "Error: cannot run 'for_Hans_with_love.sh'" >> $LOG; CleanUp; exit 1; } 
  
  [ -f ./configure ] || { echo "Error: cannot find './configure'" >> $LOG; return 1; } 

  echo "Running './configure \"$CFGOPTIONS\" \"$@\"'... " >> $LOG
  ./configure "$CFGOPTIONS" "$@" 1>> $LOG 2>&1  || { echo "Error: could not run './configure \"$CFGOPTIONS\" \"$@\"'" >> $LOG; return 1; } 

  echo "Making... " >> $LOG
  make "$MKOPTIONS" 1>> $LOG 2>&1  || { echo "Error: could not run 'make \"$MKOPTIONS\"'"5 >> $LOG; return 1; } 
}

Check()
{
  echo "Checking... " >> $LOG
  make -k -i check 1>> $LOG 2>&1

  echo "Test Result: $?" >> $LOG

  [ -x libpolys/tests ] && { echo "The content of the test directory: " >> $LOG; ls -la libpolys/tests 1>> $LOG 2>&1; }
}


Reset()
{
  echo "git reset/clean to the untouched state... " >> $LOG
  git reset --hard HEAD 1>> $LOG 2>&1 || echo "Error: could not git reset to HEAD..." >> $LOG 
  git clean -f -d -x 1>> $LOG 2>&1 || echo "Error: could not git clean..." >> $LOG 

##  #should be VERY clean now...
##  git status -uall  >> $LOG
}


echo "Creating empty tempdir: $SW" >> $LOG
[ -d $SW ] && { echo "Error: $SW exists... cleaning up..." >> $LOG; CleanUp; }

mkdir -p $SW || { echo "Error: cannot create tempdir: $SW" >> $LOG;  exit 1; }

[ -d $SW ] || { echo "Error: cannot find tempdir '$SW'" >> $LOG; CleanUp; exit 1; } 
[ ! -x $SW ] && { chmod u+rwx $SW 1>> $LOG 2>&1 || { echo "Error: cannot set rxw permissions for $SW" >> $LOG;  exit 1; }; }

echo "Clonning... " >> $LOG
git clone -v -b $BRANCH --depth 1 -- git://git.berlios.de/singular $SW 1>> $LOG 2>&1 || { echo "Error: cannot git clone..." >> $LOG; CleanUp; exit 1; } 

cd $SW  || { echo "Error: cannot cd to the tempdir: $SW" >> $LOG; CleanUp; exit 1; } 

# latest commit?
git log -1 HEAD >> $LOG 

[ -x ./for_Hans_with_love.sh ] || { echo "Error: cannot find './for_Hans_with_love.sh '" >> $LOG; CleanUp; exit 1; } 


# test two cases dynamic & static:
#  --enable-p-procs-static Enable statically compiled p_Procs-modules
#  --enable-p-procs-dynamic Enable dynamically compiled p_Procs-modules

echo "Trying static version... " >> $LOG
Build "--enable-p-procs-static" || { echo "Error: could not build with '--enable-p-procs-static'" >> $LOG; } && Check

# return git repo to the untouched state:
Reset

echo "Trying dynamic version... " >> $LOG
Build "--enable-p-procs-dynamic" || { echo "Error: could not build with '--enable-p-procs-dynamic'" >> $LOG; } && Check

cd - || { CleanUp; exit 1; } 

CleanUp && exit 0 || exit 1
