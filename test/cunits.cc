#include "units.hpp"

void
test1()
{
  // *****************************************************************
  // min(i) < min(j)
  // *****************************************************************

  // max(i) < max(j)
  {
    // units: 012345
    // i:     ***
    // j1:     ***
    // j2:      **
    // j3:       **
    CU i(0, 3);
    CU j1(1, 4);
    CU j2(2, 4);
    CU j3(3, 5);
    assert(i < j1);
    assert(i < j2);
    assert(i < j3);
  }
}

int
main()
{
  test1();
}