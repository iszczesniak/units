#include "units.hpp"

void
test1()
{
  CU cu13(1, 4);
  CU cu24(2, 5);
  CU cu23(2, 4);
  CU cu14(1, 5);
  CU cu12(1, 3);

  assert(cu13 < cu24);
  assert(cu13 < cu23);
  assert(cu14 < cu23);

  assert(cu12 < cu13);
  assert(!(cu12 < cu12));
  assert(!(cu13 < cu12));

  assert(!(cu23 < cu14));
  assert(!(cu23 < cu13));
  assert(!(cu24 < cu13));
}

int
main()
{
  test1();
}
