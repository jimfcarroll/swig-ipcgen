/* ------------------------------------------------------------
 * ipcgen.swg
 *
 * IpcGen configuration module.
 * ------------------------------------------------------------ */

//%include <swig.swg>

%{
#include "hand/MessageChannel.h"
%}

%typemap(in) bool               %{ $1 = ($1_ltype)params.getBool(); %}
%typemap(in) char               %{ $1 = ($1_ltype)params.getChar(); %}
%typemap(in) signed char        %{ $1 = ($1_ltype)params.getChar(); %}
%typemap(in) unsigned char      %{ $1 = ($1_ltype)params.getChar(); %}
%typemap(in) short              %{ $1 = ($1_ltype)params.getShort(); %}
%typemap(in) unsigned short     %{ $1 = ($1_ltype)params.getShort(); %}
%typemap(in) int                %{ $1 = ($1_ltype)params.getInt(); %}
%typemap(in) unsigned int       %{ $1 = ($1_ltype)params.getInt(); %}
%typemap(in) long               %{ $1 = ($1_ltype)params.getLong(); %}
%typemap(in) unsigned long      %{ $1 = ($1_ltype)params.getLong(); %}
%typemap(in) long long          %{ $1 = ($1_ltype)params.getLongLong(); %}
%typemap(in) unsigned long long %{ $1 = ($1_ltype)params.getLongLong(); %}
%typemap(in) float              %{ $1 = ($1_ltype)params.getFloat(); %}
%typemap(in) double             %{ $1 = ($1_ltype)params.getDouble(); %}

%typemap(in) char * %{ $1 = ($1_ltype)params.getCharPointerDirect(); %}

%typemap(out) bool               %{ ret.putBool($1); %}
%typemap(out) char               %{ ret.putChar($1); %}
%typemap(out) signed char        %{ ret.putChar($1); %}
%typemap(out) unsigned char      %{ ret.putChar((char)$1); %}
%typemap(out) short              %{ ret.putShort($1); %}
%typemap(out) unsigned short     %{ ret.putShort((short)$1); %}
%typemap(out) int                %{ ret.putInt($1); %}
%typemap(out) unsigned int       %{ ret.putInt((int)$1); %}
%typemap(out) long               %{ ret.putLong($1); %}
%typemap(out) unsigned long      %{ ret.putLong((long)$1); %}
%typemap(out) long long          %{ ret.putLongLong($1); %}
%typemap(out) unsigned long long %{ ret.putLongLong((long long)$1); %}
%typemap(out) float              %{ ret.putFloat($1); %}
%typemap(out) double             %{ ret.putDouble($1); %}

%typemap(out) char * %{ ret.putString($1); %}
%typemap(out) std::string %{ ret.putString($1.c_str()); %}

/* Generic pointers and references */
%typemap(in) SWIGTYPE * %{ $1 = ($1_ltype)params.getPointer(); %}
%typemap(in) SWIGTYPE & %{ $1 = ($1_ltype)params.getPointer(); %}


