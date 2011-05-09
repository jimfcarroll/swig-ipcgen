#!/bin/sh

SWIGINC=/home/jim/src/swig/Source

g++ -fPIC -DHAVE_CONFIG_H   -I$SWIGINC/Include -I$SWIGINC/CParse -I$SWIGINC/Include -I$SWIGINC/DOH -I$SWIGINC/CParse -I$SWIGINC/Preprocessor -I$SWIGINC/Swig -I$SWIGINC/Modules -g -ansi -pedantic -c -o ipcgen.o ./ipcgen.cxx

#g++ -fPIC -DHAVE_CONFIG_H   -I$SWIGINC/Include -I$SWIGINC/CParse -I$SWIGINC/Include -I$SWIGINC/DOH -I$SWIGINC/CParse -I$SWIGINC/Preprocessor -I$SWIGINC/Swig -I$SWIGINC/Modules -g -ansi -pedantic -c -o python.o ./python.cxx

g++ -fPIC -DHAVE_CONFIG_H   -I$SWIGINC/Include -I$SWIGINC/CParse -I$SWIGINC/Include -I$SWIGINC/DOH -I$SWIGINC/CParse -I$SWIGINC/Preprocessor -I$SWIGINC/Swig -I$SWIGINC/Modules -g -ansi -pedantic -c -o trace.o ./trace.cpp

g++ -shared -Wl,-soname,libipcgen.so -o libipcgen.so ./ipcgen.o ./trace.o
