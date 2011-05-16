#!/bin/sh -x

SWIGROOT=/home/jim/src/swig

#$SWIGROOT/swig -c++ -outdir ./ -o ./AddonModuleXbmcgui_wrap.cpp -plugin ./libipcgen.so -pluginlang ipcgen -I./Lib/ipcgen -I$SWIGROOT/Lib -I/home/jim/src/xbmc/xbmc/interfaces/python/module ~/src/xbmc/xbmc/interfaces/python/module/swig/AddonModuleXbmcgui.i

#-debug-classes -debug-module -debug-symtabs -debug-symbols -debug-csymbols -debug-lsymbols -debug-tags -debug-template -debug-top -debug-typedef -debug-typemap -debug-tmsearch -debug-tmused 

#$SWIGROOT/swig -v -debug-classes -debug-module 1 -debug-symtabs -debug-symbols -debug-csymbols -debug-lsymbols -debug-tags -debug-template -debug-top 1 -debug-typedef -debug-typemap -debug-tmsearch -debug-tmused -c++ -outdir ./ -o ./test_wrap.cpp -plugin ./libipcgen.so -pluginlang ipcgen -I$SWIGROOT/Lib/python -I./Lib/ipcgen -I$SWIGROOT/Lib test.i

$SWIGROOT/swig -xmlout ./test.xml -v -c++ -outdir ./ -o ./test_parent_wrap.cpp -plugin ./libipcgenp.so -pluginlang ipcgen -I./Lib/ipcgen -I$SWIGROOT/Lib test.i
#$SWIGROOT/swig -xmlout ./test.xml -v -c++ -outdir ./ -o ./test_child_wrap.cpp -plugin ./libipcgenc.so -pluginlang ipcgen -I./Lib/ipcgen -I$SWIGROOT/Lib test.i

#$SWIGROOT/swig -xmlout ./test.xml -v -c++ -outdir ./ -o ./test_wrap.cpp -plugin ./libipcgen.so -pluginlang ipcgen -I./Lib/ipcgen -I$SWIGROOT/Lib  ~/src/xbmc/xbmc/interfaces/python/module/swig/AddonModuleXbmcgui.i

#$SWIGROOT/swig -xmlout ./test.xml -v -c++ -outdir ./ -o ./test_wrap.cpp -csharp -I$SWIGROOT/Lib -I$SWIGROOT/Lib/csharp test.i


