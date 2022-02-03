#include "units.hpp"

// Here we exhaustively test the < operator.  These are the
// possibilities of comparing i < j:
//
// 1. If min(i) < min(j): always i < j, regardless of:
//
//   a. max(i) < max(j): incomparable, but we say i < j
//   b. max(i) == max(j): i is better than j, so i < j
//   c. max(i) > max(j): i is better than j, so i < j
//
// 2. If min(i) == min(j), and:
//
//   a. max(i) < max(j): j properly includes i, and so j < i
//
//   b. max(i) == max(j): i == j
//
//   c. max(i) > max(j): same as 2a
//
// 3. If min(i) > min(j), and:
//
//   a. max(i) < max(j): same as 1c
//
//   b. max(i) == max(j): same as 1b
//
//   c. max(i) > max(j): same as 1a

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
    assert(i < j1); // cases 1a, 
    assert(i < j2); // case 1b
    assert(i < j3); // case 1c
    assert(!(j1 < i)); // case 3c
    assert(!(j2 < i)); // case 3b
    assert(!(j3 < i)); // case 3a
  }

  // b. max(i) == max(j)
  {
    // units: 012
    // i:     **
    // j:      *
    CU i(0, 2);
    CU j(1, 2);
    assert(i < j);
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
    assert(!(i < j));
  }

  // b. max(i) == max(j)
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
