#include "units.hpp"

// *****************************************************************
// min(i) < min(j)
// *****************************************************************
void
test_min_lt()
{
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
    assert(!(j1 < i));
    assert(!(j2 < i));
    assert(!(j3 < i));
  }

  // max(i) == max(j)
  {
    // units: 012
    // i:     **
    // j:      *
    CU i(0, 2);
    CU j(1, 2);
    assert(i < j);
    assert(!(j < i));
  }

  // max(i) > max(j)
  {
    // units: 0123
    // i:     ***
    // j:      *
    CU i(0, 3);
    CU j(1, 2);
    assert(i < j);
    assert(!(j < i));
  }
}

// *****************************************************************
// min(i) == min(j)
// *****************************************************************
void
test_min_eq()
{
  // max(i) < max(j)
  {
    // units: 012
    // i:     *
    // j:     **
    CU i(0, 1);
    CU j(0, 2);
    assert(j < i);
    assert(!(i < j));
  }

  // max(i) == max(j)
  {
    // units: 01
    // i:     *
    CU i(0, 1);
    assert(i == i);
    assert(!(i < i));
  }
}

int
main()
{
  test_min_lt();
  test_min_eq();
}
