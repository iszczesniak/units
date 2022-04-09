#include "units.hpp"

#include <list>

using namespace std;

// Make sure that i < j.
template <typename T>
void
test_less(const T &i, const T &j)
{
  assert(i < j);
  assert(i <= j);
  assert(i != j);
  assert(!(i == j));
  assert(!(i > j));
  assert(!(i >= j));
}

// Make sure that i == j.
template <typename T>
void
test_equal(const T &i, const T &j)
{
  assert(i == j);
  assert(!(i != j));
  assert(i <= j);
  assert(i >= j);
  assert(!(i < j));
  assert(!(i > j));
}

// Make sure that i > j.
template <typename T>
void
test_greater(const T &i, const T &j)
{
  test_less(j, i);
}

void
test_comparable(const CU &ri, const CU &rj)
{
  assert(includes(ri, rj) || includes(rj, ri));
}

void
test_incomparable(const CU &ri, const CU &rj)
{
  assert(!includes(ri, rj));
  assert(!includes(rj, ri));
}

// Returns a list of rj such that: ri < rj
auto
greater_RIs(const CU &ri)
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
    test_less(ri, rj);
    test_incomparable(ri, rj);
  }
  // Row 1, column 2.
  {
    CU rj(ri.min() + 1, ri.max());
    test_less(ri, rj);
    test_comparable(ri, rj);
  }
  // Row 1, column 3.
  {
    CU rj(ri.min() + 1, ri.max() - 1);
    test_less(ri, rj);
    test_comparable(ri, rj);
  }

  // -----------------------------------------------------------------
  // Row 2, column 1.
  {
    CU rj(ri.min(), ri.max() + 1);
    test_greater(ri, rj);
    test_comparable(ri, rj);
  }
  // Row 2, column 2.
  {
    CU rj(ri.min(), ri.max());
    test_equal(ri, rj);
    test_comparable(ri, rj);
  }
  // Row 2, column 3.
  {
    CU rj(ri.min(), ri.max() - 1);
    test_less(ri, rj);
    test_comparable(ri, rj);
  }

  // -----------------------------------------------------------------
  // Row 3, column 1.
  {
    CU rj(ri.min() - 1, ri.max() + 1);
    test_greater(ri, rj);
    test_comparable(ri, rj);
  }
  // Row 3, column 2.
  {
    CU rj(ri.min() - 1, ri.max());
    test_greater(ri, rj);
    test_comparable(ri, rj);
  }
  // Row 3, column 3.
  {
    CU rj(ri.min() - 1, ri.max() - 1);
    test_greater(ri, rj);
    test_incomparable(ri, rj);
  }
}
// *****************************************************************
// Test transitivity.
// *****************************************************************

void
test_transitivity()
{
  // This CU could be any.
  CU ri(10, 20);

  for(const auto &rj: greater_RIs(ri))
    for(const auto &rk: greater_RIs(rj))
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

int
main()
{
  test_relations();
  test_transitivity();
}
