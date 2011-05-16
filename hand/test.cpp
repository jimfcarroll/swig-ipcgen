
#include <stdio.h>
#include "test.h"


std::string func(int param)
{
  printf("Passed to func: %i\n", param);

  return std::string("hello there");
}

int func2(const char* param)
{
  printf("Passed to func2: %s\n", param);
  return 42;
}

