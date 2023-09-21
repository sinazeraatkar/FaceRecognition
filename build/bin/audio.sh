#! /bin/bash

if [ ! -e /tmp/embsys ]
then
    mkdir /tmp/embsys
fi
arecord -f cd -s 500 /tmp/embsys/ex.wav &> /dev/null
rm -f /tmp/embsys/file.out
sox /tmp/embsys/ex.wav -n stat &> /tmp/embsys/file.out
