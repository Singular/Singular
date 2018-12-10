#!/bin/sh
VERSION=`flex --version |sed -e "s/^.*version //"|sed -e "s/^flex //"`
LV=`echo $VERSION|sed -e "s/\.[0-9]*\.[0-9]*\$//"`
MIDV=`echo $VERSION|sed -e "s/^[0-9]*\.//"|sed -e "s/\.[0-9]*\$//"`
TV=`echo $VERSION|sed -e "s/^[0-9]*\.[0-9]*\\.//"`
#echo $LV $MIDV $TV
#goodversion=""
if [ $LV -lt 2 ];
    then goodversion=true
fi
if [ $LV -eq 2 ];
    then
    if [ $MIDV -lt 5 ];
	then goodversion=true
    fi
    if [ $MIDV -eq 5 ];
	then
	if [ $TV -le 4 ];
	    then goodversion=true;
	fi
    fi
fi

#echo $goodversion
if [ -z $goodversion ];
    then
    #echo "bad version, trying to use touch"
    for arg in $1 $2 $3 $4 $5 $6 $7 $8 $9
      do
      #echo loop
      if [ -z $arg ];
	  then
	  dummy=0;
      else
	  #echo `echo $arg|sed -e "s/-.

*/SPECIALVALUE/"`
	  if  [ SPECIALVALUE = `echo $arg|sed -e "s/-.

*/SPECIALVALUE/"` ];

	      then
	      dummy=0;
	  else
	      file=$arg

	  fi
      fi
    done
    #echo file arg is $file
    file2=`echo $file|sed -e "s/\.l$/\.cc/"`
    #echo $file2
    if [ -r $file2 ];
	then
#	file3=`echo $file2|sed -e "s/\$/.tmp/"`
#	cp $file2 $file3
#	cat $file3
	cat $file2
    else
	#echo "file does not exist"
	exit 1;
    fi

else
    #echo "good version";
    flex $1 $2 $3 $4 $5 $6 $7 $8 $9
fi
