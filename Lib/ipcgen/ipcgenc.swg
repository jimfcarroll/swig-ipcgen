/* ------------------------------------------------------------
 * ipcgen.swg
 *
 * IpcGen configuration module.
 * ------------------------------------------------------------ */

//%include <swig.swg>

%{
#include "hand/MessageChannel.h"
%}

%typemap(in) char, 
             signed char, 
             unsigned char, 
             short, 
             unsigned short, 
             int, 
             unsigned int, 
             long, 
             unsigned long, 
             long long, 
             unsigned long long, 
             float, 
             double
%{ $1 = ($1_ltype)$input; %}

%typemap(in) char * %{ $1 = ($1_ltype)$input; %}

%typemap(out) bool               %{ $result = $1; %}
%typemap(out) char               %{ $result = $1; %}
%typemap(out) signed char        %{ $result = $1; %}
%typemap(out) unsigned char      %{ $result = $1; %}
%typemap(out) short              %{ $result = $1; %}
%typemap(out) unsigned short     %{ $result = $1; %}
%typemap(out) int                %{ $result = $1; %}
%typemap(out) unsigned int       %{ $result = $1; %}
%typemap(out) long               %{ $result = $1; %}
%typemap(out) unsigned long      %{ $result = (unsigned long)$1; %}
%typemap(out) long long          %{ $result = $1; %}
%typemap(out) unsigned long long %{ $result = $1; %}
%typemap(out) float              %{ $result = $1; %}
%typemap(out) double             %{ $result = $1; %}

%typemap(imtype) int %{ message.putInt((int)$1); %}

%typemap(imtype) bool,               const bool &               "bool"
%typemap(imtype) char,               const char &               "char"
%typemap(imtype) signed char,        const signed char &        "sbyte"
%typemap(imtype) unsigned char,      const unsigned char &      "byte"
%typemap(imtype) short,              const short &              "short"
%typemap(imtype) unsigned short,     const unsigned short &     "ushort"
//%typemap(imtype) int,                const int &                "int"
%typemap(imtype) unsigned int,       const unsigned int &       "uint"
%typemap(imtype) long,               const long &               "int"
%typemap(imtype) unsigned long,      const unsigned long &      "uint"
%typemap(imtype) long long,          const long long &          "long"
%typemap(imtype) unsigned long long, const unsigned long long & "ulong"
%typemap(imtype) float,              const float &              "float"
%typemap(imtype) double,             const double &             "double"
%typemap(imtype) void                                           "void"
