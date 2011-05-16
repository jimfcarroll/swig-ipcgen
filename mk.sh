#!/bin/sh

SWIGINC=/home/jim/src/swig/Source

g++ -fPIC -DHAVE_CONFIG_H   -I$SWIGINC/Include -I$SWIGINC/CParse -I$SWIGINC/Include -I$SWIGINC/DOH -I$SWIGINC/CParse -I$SWIGINC/Preprocessor -I$SWIGINC/Swig -I$SWIGINC/Modules -g -ansi -pedantic -c -o ipcgenc.o ./ipcgenc.cxx

g++ -fPIC -DHAVE_CONFIG_H   -I$SWIGINC/Include -I$SWIGINC/CParse -I$SWIGINC/Include -I$SWIGINC/DOH -I$SWIGINC/CParse -I$SWIGINC/Preprocessor -I$SWIGINC/Swig -I$SWIGINC/Modules -g -ansi -pedantic -c -o ipcgenp.o ./ipcgenp.cxx

#g++ -fPIC -DHAVE_CONFIG_H   -I$SWIGINC/Include -I$SWIGINC/CParse -I$SWIGINC/Include -I$SWIGINC/DOH -I$SWIGINC/CParse -I$SWIGINC/Preprocessor -I$SWIGINC/Swig -I$SWIGINC/Modules -g -ansi -pedantic -c -o python.o ./python.cxx

g++ -fPIC -DHAVE_CONFIG_H   -I$SWIGINC/Include -I$SWIGINC/CParse -I$SWIGINC/Include -I$SWIGINC/DOH -I$SWIGINC/CParse -I$SWIGINC/Preprocessor -I$SWIGINC/Swig -I$SWIGINC/Modules -g -ansi -pedantic -c -o trace.o ./trace.cpp

g++ -shared -Wl,-soname,libipcgenp.so -o libipcgenp.so ./ipcgenp.o ./trace.o
g++ -shared -Wl,-soname,libipcgenc.so -o libipcgenc.so ./ipcgenc.o ./trace.o
