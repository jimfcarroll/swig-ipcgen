
%module(directors="1") test

%include <std_string.i>

namespace testns
{
  int func(const char* param);

  std::string func2(int param);
  void func3(int param1, const char* param2, double param3, long long param4);

  class MyClass;

  void func4(MyClass* object);

  std::string func5(int param1, const char* param2, double param3, long long param4);

  void func6(MyClass& val);

  class A
  {
  public:
    int foo(int);
  };
}
