
%module(directors="1") test

//%begin %{
//  // INSERT 1
//  %}
//
//%runtime %{
//  // INSERT 2
//  %}
//
//%header %{
//  // INSERT 3
//  %}
//
//%wrapper %{
//  // INSERT 4
//  %}
//
//%init %{
//  // INSERT 5
//  %}
//
//// typemaps
//%include <std_string.i>

//%feature("director") outer::testNamespace::A;
//
//namespace outer
//{
//  namespace testNamespace
//  {
//
//    class A
//    {
//    public:
//      A() {}
//      virtual ~A() {}
//
//      virtual void func1(std::string str);
//      void func2();
//      int func3(double d);
//    };
////
////    class B : public A
////    {
////    public:
////      B(int f) {}
////      virtual ~B() {}
////
////      virtual void func(std::string str);
////    };
////
////    void func4();
////
//  }
////
////  int func5(aruggala junk);
//}

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


int func(const char* param);

