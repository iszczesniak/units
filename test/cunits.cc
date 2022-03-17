#include "units.hpp"

#include <list>

using namespace std;

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
    assert(!(i > j1));
    assert(!(i > j2));
    assert(!(i > j3));
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
    assert(!(i > i));
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
    assert(!(i > j));
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
    assert(i > j);

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

  // c. max(i) == max(j)
  // The reverse of point a.
}

// Returns a list of four CUs that are worse than the argument.
list<CU>
worse(const CU &ri)
{
  list<CU> l;

  // For case 1.a.
  l.push_back(CU(ri.min() + 1, ri.max() + 1));
  // For case 1.b.
  l.push_back(CU(ri.min() + 1, ri.max()));
  // For case 1.c.
  l.push_back(CU(ri.min() + 1, ri.max() - 1));
  // For case 2.c.
  l.push_back(CU(ri.min(), ri.max() - 1));

  return l;
}

// *****************************************************************
// transitivity
// *****************************************************************
void
test_transitivity()
{
  // This CU could be any.
  CU ri(10, 20);

  for(const auto &rj: worse(ri))
    for(const auto &rk: worse(rj))
      {
        assert(ri < rj && rj < rk);
        assert(ri < rk);

        // Test the default implementation of >.
        assert(rk > rj && rj > ri);
        assert(rk > ri);

        // Test the default implementation of <=.
        assert(ri <= rj && rj <= rk);
        assert(ri <= rk);

        // Test the default implementation of >=.
        assert(rk >= rj && rj >= ri);
        assert(rk >= ri);
      }
}

// *****************************************************************
// Test all relations
// *****************************************************************

void
test_relations()
{
  CU ri(10, 20);

  // -----------------------------------------------------------------
  // Row 1, column 1.
  {
    CU rj(ri.min() + 1, ri.max() + 1);
    assert(ri < rj);
  }
  // Row 1, column 2.
  {
    CU rj(ri.min() + 1, ri.max());
    assert(ri < rj);
  }
  // Row 1, column 3.
  {
    CU rj(ri.min() + 1, ri.max() - 1);
    assert(ri < rj);
  }

  // -----------------------------------------------------------------
  // Row 2, column 1.
  {
    CU rj(ri.min(), ri.max() + 1);
    assert(ri > rj);
  }
  // Row 2, column 2.
  {
    CU rj(ri.min(), ri.max());
    assert(ri == rj);
  }
  // Row 2, column 3.
  {
    CU rj(ri.min(), ri.max() - 1);
    assert(ri < rj);
  }

  // -----------------------------------------------------------------
  // Row 3, column 1.
  {
    CU rj(ri.min() - 1, ri.max() + 1);
    assert(ri > rj);
  }
  // Row 3, column 2.
  {
    CU rj(ri.min() - 1, ri.max());
    assert(ri > rj);
  }
  // Row 3, column 3.
  {
    CU rj(ri.min() - 1, ri.max() - 1);
    assert(ri > rj);
  }
}

int
main()
{
  test_min_lt();
  test_min_eq();
  test_transitivity();
  test_relations();
}
