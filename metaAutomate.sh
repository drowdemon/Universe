#!/bin/bash

otherDir=$PWD

#scriptDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

autoscan
mv configure.scan configure.ac
python /home/marc/Coding/metaAutomate/editConfAc.py $PWD
python /home/marc/Coding/Universe/createMakeAm.py $PWD
aclocal
autoconf
automake -a
./configure
python /home/marc/Coding/metaAutomate/changeMake.py
