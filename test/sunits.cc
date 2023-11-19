#include "units.hpp"

#include <cassert>

void
test1()
{
  SU s{{100, 101}, {200, 202}, {300, 303}};
  assert(s.size() == 6);
}

void
test_includes_cu()
{
  SU s0{};
  assert(!includes(s0, CU(0, 1)));  

  SU s1{{10, 20}};
  assert(!includes(s1, CU(5, 15)));
  assert(!includes(s1, CU(9, 10)));

  assert(includes(s1, CU(10, 11)));
  assert(includes(s1, CU(14, 16)));
  assert(includes(s1, CU(19, 20)));
  assert(includes(s1, CU(10, 20)));

  assert(!includes(s1, CU(20, 21)));
  assert(!includes(s1, CU(15, 25)));

  SU s2{{10, 20}, {30, 40}};
  assert(!includes(s2, CU(5, 15)));
  assert(!includes(s2, CU(9, 10)));

  assert(includes(s2, CU(10, 11)));
  assert(includes(s2, CU(14, 16)));
  assert(includes(s2, CU(19, 20)));
  assert(includes(s2, CU(10, 20)));

  assert(!includes(s2, CU(20, 21)));
  assert(!includes(s2, CU(15, 25)));
  assert(!includes(s2, CU(20, 30)));
  assert(!includes(s2, CU(25, 35)));
  assert(!includes(s2, CU(29, 30)));

  assert(includes(s2, CU(30, 31)));
  assert(includes(s2, CU(34, 36)));
  assert(includes(s2, CU(39, 40)));
  assert(includes(s2, CU(30, 40)));

  assert(!includes(s2, CU(35, 45)));
  assert(!includes(s2, CU(40, 41)));
}

int
main()
{
  test1();
  test_includes_cu();
}
