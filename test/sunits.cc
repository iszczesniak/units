#include "units.hpp"

#include <cassert>

void
test1()
{
  SU s{{100, 101}, {200, 202}, {300, 303}};
  assert(s.r2a(0) == 100);
  assert(s.r2a(1) == 200);
  assert(s.r2a(2) == 201);
  assert(s.r2a(3) == 300);
  assert(s.r2a(4) == 301);
  assert(s.r2a(5) == 302);
}

int
main()
{
  test1();
}
