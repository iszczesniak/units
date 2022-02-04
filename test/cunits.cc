#include "units.hpp"

// Here we exhaustively test the < operator.  These are the
// possibilities of comparing i < j:
//
// 1. If min(i) < min(j): always i < j, regardless of:
//
//   a. max(i) < max(j): i and j are incomparable, but we say i < j
//                       because we need to establish a weak strong
//                       ordering
//
//   b. max(i) == max(j): i < j because i properly includes j
//
//   c. max(i) > max(j): i < j because i properly includes j
//
// 2. If min(i) == min(j), and:
//
//   a. max(i) < max(j): j properly includes i, and so j < i
//
//   b. max(i) == max(j): i == j
//
//   c. max(i) > max(j): same as 2a, just swap i and j
//
// 3. If min(i) > min(j), and:
//
//   a. max(i) < max(j): same as 1c, just swap i and j
//
//   b. max(i) == max(j): same as 1b, just swap i and j
//
//   c. max(i) > max(j): same as 1a, just swap i and j

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
