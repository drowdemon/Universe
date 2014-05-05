#!/usr/bin/python

import sys
import commands

if(len(sys.argv)!=2):
    sys.stdout.write("Need argument: file path\n")
    exit(5)
x = sys.argv[1]
#sys.stdout.write(x)
dirname = ""
for i in range(0,len(x)):
    if(x[len(x)-1-i]=='/'):
        dirname = x[len(x)-i:]
        break
#sys.stdout.write("\n" + dirname + "\n")

f = open('Makefile.am', 'w+')
f.write("""AUTOMAKE_OPTIONS = foreign

CXXFLAGS=-Wall -g3 -O0 -Wextra -Wfloat-equal -Wconversion -std=c++0x
LDFLAGS=

bin_PROGRAMS = """ + dirname  + "\n" + dirname +"_SOURCES = ")

allFiles = commands.getoutput("find . | grep .cpp$")
print allFiles

for c in allFiles:
    if(c=='\n'):
        f.write(" ")
    else:
        f.write(c)
