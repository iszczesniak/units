#include "units.hpp"

// Test exhaustively the < operator with all possible combinations, as
// detailed in the implementation in the header file.

// *****************************************************************
// 1. min(i) < min(j)
// *****************************************************************

void
test_min_lt()
{
  // a. max(i) < max(j)
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

    // Asymmetry
    assert(!(j1 < i));
    assert(!(j2 < i));
    assert(!(j3 < i));
  }

  // b. max(i) == max(j)
  {
    // units: 012
    // i:     **
    // j:      *
    CU i(0, 2);
    CU j(1, 2);
    assert(i < j);

    // Asymmetry
    assert(!(j < i));
  }

  // c. max(i) > max(j)
  {
    // units: 0123
    // i:     ***
    // j:      *
    CU i(0, 3);
    CU j(1, 2);
    assert(i < j);

    // Asymmetry
    assert(!(j < i));
  }
}

// *****************************************************************
// 2. min(i) == min(j)
// *****************************************************************
void
test_min_eq()
{
  // a. max(i) < max(j)
  {
    // units: 012
    // i:     *
    // j:     **
    CU i(0, 1);
    CU j(0, 2);
    assert(j < i);

    // Asymmetry
    assert(!(i < j));
  }

  // b. max(i) == max(j)
  {
    // units: 01
    // i:     *
    CU i(0, 1);
    assert(i == i);

    // Asymmetry
    assert(!(i < i));
  }
}

int
main()
{
  test_min_lt();
  test_min_eq();
}
