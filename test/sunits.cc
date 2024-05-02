#include "units.hpp"

#include <cassert>

void
test_includes()
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
  assert((SU{{0, 1}} < SU{}));
  // A subset is worse.
  assert((SU{{0, 3}} < SU{{1, 2}}));

  assert((SU{{0, 3}} < SU{{0, 1}, {2, 3}}));
}

int
main()
{
  // test_includes();
  // test_insert();
  // test_remove();
  // test_size();
  test_less();
}
