#include "units.hpp"

#include <cassert>

void
test1()
{
  SU s{{100, 101}, {200, 202}, {300, 303}};
  assert(s.size() == 6);
}

int
main()
{
  test1();
}
