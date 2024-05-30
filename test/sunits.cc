#include "helpers.hpp"
#include "units.hpp"

#include <cassert>

void
test_includes_interval()
{
  SU s0{};
  assert(!includes(s0, {0, 1}));  

  SU s1{{10, 20}};
  assert(!includes(s1, {5, 15}));
  assert(!includes(s1, {9, 10}));

  assert(includes(s1, {10, 11}));
  assert(includes(s1, {14, 16}));
  assert(includes(s1, {19, 20}));
  assert(includes(s1, {10, 20}));

  assert(!includes(s1, {20, 21}));
  assert(!includes(s1, {15, 25}));

  SU s2{{10, 20}, {30, 40}};
  assert(!includes(s2, {5, 15}));
  assert(!includes(s2, {9, 10}));

  assert(includes(s2, {10, 11}));
  assert(includes(s2, {14, 16}));
  assert(includes(s2, {19, 20}));
  assert(includes(s2, {10, 20}));

  assert(!includes(s2, {20, 21}));
  assert(!includes(s2, {15, 25}));
  assert(!includes(s2, {20, 30}));
  assert(!includes(s2, {25, 35}));
  assert(!includes(s2, {29, 30}));

  assert(includes(s2, {30, 31}));
  assert(includes(s2, {34, 36}));
  assert(includes(s2, {39, 40}));
  assert(includes(s2, {30, 40}));

  assert(!includes(s2, {35, 45}));
  assert(!includes(s2, {40, 41}));
}

void
test_includes_intervals()
{
  // The empty includes the empty.
  assert(includes(SU{}, SU{}));

  // The non-empty includes the empty.
  assert(includes(SU{CU(0, 1)}, SU{}));

  // A set should include the same set.
  SU su1{{0, 5}, {10, 15}, {20, 25}};

  assert(includes(SU{{0, 5}}, SU{{0, 5}}));
  assert(includes(SU{{0, 5}, {10, 15}},
                  SU{{0, 5}, {10, 15}}));
  assert(includes(su1, su1));

  // Subsets of su1.
  assert(includes(su1, SU{{0, 5}}));
  assert(includes(su1, SU{{1, 5}}));
  assert(includes(su1, SU{{0, 4}}));
  assert(includes(su1, SU{{1, 4}}));

  assert(includes(su1, SU{{10, 15}}));
  assert(includes(su1, SU{{11, 15}}));
  assert(includes(su1, SU{{10, 14}}));
  assert(includes(su1, SU{{11, 14}}));

  assert(includes(su1, SU{{20, 25}}));
  assert(includes(su1, SU{{21, 25}}));
  assert(includes(su1, SU{{20, 24}}));
  assert(includes(su1, SU{{21, 24}}));

  SU sua{{16, 17}, {22, 23}, {27, 28}, {29, 30}, {31, 32}, {33, 34},
         {35, 36}, {44, 45}, {50, 51}, {63, 64}, {65, 66}, {67, 68},
         {84, 85}, {86, 87}, {103, 104}, {106, 107}, {136, 137},
         {150, 151}, {156, 157}};

  SU sub{{29, 30}, {44, 45}, {50, 51}, {67, 68}, {136, 137}};

  assert(includes(sua, sub));
}

void
test_insert()
{
  SU s;
  assert(!includes(s, {10, 11}));

  // Insert something into an empty set.
  s.insert({10, 11});
  assert(includes(s, {10, 11}));

  // Insert some preceding units.
  s.insert({9, 10});
  assert(includes(s, {9, 11}));

  // Insert some trailing units.
  s.insert({11, 12});
  assert(includes(s, {9, 12}));

  // We do not allow to include already existing units.  That would
  // complicate the code, and I don't need this.
  // s.insert({5, 15});

  // Insert another interval.
  s.insert({20, 30});
  assert(includes(s, {20, 30}));
  assert(!includes(s, {19, 20}));

  // Insert some preceding units.
  s.insert({19, 20});
  assert(includes(s, {19, 21}));

  // Insert some trailing units.
  s.insert({30, 31});
  assert(includes(s, {19, 31}));

  // Fill in the gap to have a single interval.
  s.insert({12, 19});
  assert(includes(s, {9, 31}));
}

void
test_remove()
{
  SU s{{9, 31}};

  // Create a large gap.
  s.remove({12, 19});
  assert(includes(s, {9, 12}));
  assert(!includes(s, {12, 19}));
  assert(includes(s, {19, 31}));

  s.remove({30, 31});
  assert(includes(s, {19, 30}));

  s.remove({19, 20});
  assert(!includes(s, {12, 20}));
  assert(includes(s, {20, 30}));

  s.remove({20, 30});
  assert(!includes(s, {12, 30}));
  assert(includes(s, {9, 12}));

  s.remove({11, 12});
  assert(!includes(s, {9, 12}));

  s.remove({9, 10});
  s.remove({10, 11});
  
  assert(s.empty());
}

void
test_size()
{
  SU s{{100, 101}, {200, 202}, {300, 303}};
  assert(s.size() == 6);
}

// Test <.
void
test_less()
{
  // Empty is always worse.
  assert(is_greater(SU{{0, 1}}, SU{}));
  // A subset is worse.
  assert(is_greater(SU{{0, 3}}, SU{{1, 2}}));
  // A number of subsets is worse.
  assert(is_greater(SU{{0, 3}}, SU{{0, 1}, {2, 3}}));
  // They are equal, one cannot be better than.
  assert(is_equal(SU{{0, 3}}, SU{{0, 3}}));
  // Both SUs start with the same CU, but the first has more to offer.
  assert(is_greater(SU{{0, 3}, {5, 6}}, SU{{0, 3}}));
  // They are incomparable but the first should come first.
  assert(is_greater(SU{{0, 2}}, SU{{1, 3}}));
}

int
main()
{
  test_includes_interval();
  test_includes_intervals();
  test_insert();
  test_remove();
  test_size();
  test_less();
}
