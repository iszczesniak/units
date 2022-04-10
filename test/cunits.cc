#include "helpers.hpp"

#include "units.hpp"

#include <cassert>
#include <list>

using namespace std;

bool
is_comparable(const CU &ri, const CU &rj)
{
  return includes(ri, rj) || includes(rj, ri);
}

bool
is_incomparable(const CU &ri, const CU &rj)
{
  return !includes(ri, rj) && !includes(rj, ri);
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
// Test all relations.
// *****************************************************************

void
test_relations()
{
  CU ri(10, 20);

  // -----------------------------------------------------------------
  // Row 1, column 1.
  {
    CU rj(ri.min() + 1, ri.max() + 1);
    assert(is_less(ri, rj));
    assert(is_incomparable(ri, rj));
  }
  // Row 1, column 2.
  {
    CU rj(ri.min() + 1, ri.max());
    assert(is_less(ri, rj));
    assert(is_comparable(ri, rj));
  }
  // Row 1, column 3.
  {
    CU rj(ri.min() + 1, ri.max() - 1);
    assert(is_less(ri, rj));
    assert(is_comparable(ri, rj));
  }

  // -----------------------------------------------------------------
  // Row 2, column 1.
  {
    CU rj(ri.min(), ri.max() + 1);
    assert(is_greater(ri, rj));
    assert(is_comparable(ri, rj));
  }
  // Row 2, column 2.
  {
    CU rj(ri.min(), ri.max());
    assert(is_equal(ri, rj));
    assert(is_comparable(ri, rj));
  }
  // Row 2, column 3.
  {
    CU rj(ri.min(), ri.max() - 1);
    assert(is_less(ri, rj));
    assert(is_comparable(ri, rj));
  }

  // -----------------------------------------------------------------
  // Row 3, column 1.
  {
    CU rj(ri.min() - 1, ri.max() + 1);
    assert(is_greater(ri, rj));
    assert(is_comparable(ri, rj));
  }
  // Row 3, column 2.
  {
    CU rj(ri.min() - 1, ri.max());
    assert(is_greater(ri, rj));
    assert(is_comparable(ri, rj));
  }
  // Row 3, column 3.
  {
    CU rj(ri.min() - 1, ri.max() - 1);
    assert(is_greater(ri, rj));
    assert(is_incomparable(ri, rj));
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
