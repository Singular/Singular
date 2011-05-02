#!/bin/sh

PWD=`pwd`
BRANCH="spielwiese"

# configure' options
CFGOPTIONS="--with-gmp=yes"

# computer specific make options: e.g. -j9
#MKOPTIONS="-j9"
MKOPTIONS="" 

D=`date +%y.%m.%d.%H.%M.%S`

# tempdir (absolute path!)
SW="$PWD/SW_$D.dir"

# we try to use log instead of consequent --quiet
LOG="$PWD/SW_$D.log"

# log or no log?
echo >> $LOG || LOG="/dev/null"

echo "DATE: `date`" >> $LOG
echo "HOST: `hostname`" >> $LOG
echo "SYSTEM: `uname -a`" >> $LOG 


CleanUp() 
{
  echo "Deleting tempdir: $SW" >> $LOG
  rm -f -R "$SW" 1>> $LOG 2>&1
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
[ -d $SW ] && { echo "Error: $SW exists... cleaning up..." >> $LOG; mv -f $SW "$SW.moved" 1>> $LOG 2>&1 && { rm -f -R "$SW.moved" 1>> $LOG 2>&1 || echo "Error: could not remove '$SW.moved'"; } || { echo "Error: could not rename '$SW' -> '$SW.moved'"; exit 1; }; }

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
Build "--enable-p-procs-static" && Check || { echo "Error: could not build with '--enable-p-procs-static'" >> $LOG; } 

# return git repo to the untouched state:
echo "Resetting the source directory... " >> $LOG
Reset

echo "Trying dynamic version... " >> $LOG
Build "--enable-p-procs-dynamic" && Check || { echo "Error: could not build with '--enable-p-procs-dynamic'" >> $LOG; }

cd - || { CleanUp; exit 1; } 

CleanUp || exit 1

exit 0
