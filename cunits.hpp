#ifndef CUNITS_HPP
#define CUNITS_HPP

#include <cassert>
#include <compare>
#include <concepts>
#include <iostream>
#include <list>

// Describes a resource interval [min, max), i.e., min is included,
// and max is not.  The interval endpoints are totally ordered.

// Relations between resource invervals i and j:
//
// * < is a linear ordering.
//
// * sb is a proper subset,
//
// * sp is a proper superset,
//
// * || is sb-incomparability: set inclusion relation does not hold.
//
// The < operator establishes an ordering needed for sorting in, e.g.,
// containers.  We need <, because we cannot use the inclusion
// relation: the inclusion relation is not a strict weak ordering as
// the incomparability is intransitive.
//
//               ----------------------------------------------------
//               | i.max < j.max  | i.max = j.max  | i.max > j.max  |
//               ----------------------------------------------------
// i.min < j.min | i || j | i > j | i sp j | i > j | i sp j | i > j |
//               ----------------------------------------------------
// i.min = j.min | i sb j | i < j |      i = j     | i sp j | i > j |
//               ----------------------------------------------------
// i.min > j.min | i sb j | i < j | i sb j | i < j | i || j | i < j |
//               ----------------------------------------------------
//
// Better intervals must be processed first, so the superset relation
// must imply the greater relation: if i sp j, then i > j.  However,
// there are labels that do not compare with the inclusion relation
// (the || relation), and we have to define linear ordering for them.
// We have two choices.
//
// The first choice:
//
// >>>
// <=>
// <<<
//
// i > j iff i.min < j.min or i.min == j.min and i.max > j.max
//
// The second choice:
//
// <>>
// <=>
// <<>
//
// i > j iff i.max > j.max or i.max == j.max and i.min < j.min
//
// The < relation is lexicographic: the min's are compared with < and
// max's are compared with > (note it's the reverse).  In the first
// choice the min's are compared first and the max's second, while in
// the second choice it's the other way around.
//
// The default <=> would lexicographically compare both the min's and
// the max's with <.  First min's, then max's.  Without >.

template <std::totally_ordered T>
class cunits
{
  // Lower and upper endpoints.  I would like to have them const, but
  // the default (copy and move) assignment would be ill-formed.
  T m_min, m_max;

public:
  // There is no default constructor, because it would be problematic
  // (with, e.g., comparison).

  // The only constructor.
  cunits(T min, T max): m_min(min), m_max(max)
  {
    // We disallow an empty interval.
    assert(min < max);
  }

  const T &
  min() const
  {
    return m_min;
  }

  const T &
  max() const
  {
    return m_max;
  }

  T
  size() const
  {
    return m_max - m_min;
  }

  bool
  empty() const
  {
    return !size();
  }

  constexpr bool operator == (const cunits &) const = default;
};

// The <=> comparison implements the table from the top and the first
// choice of breaking ties:
//
// i > j iff i.min < j.min or i.min == j.min and i.max > j.max
//
// And so:
//
// * i > j if:
//
//   i.min < j.min || i.min == j.min && i.max > j.max
//
// * i < j if:
//
//   i.min > j.min || i.min == j.min && i.max < j.max
//
// * i == j otherwise.

template<typename T>
constexpr auto
operator <=> (const cunits<T> &i, const cunits<T> &j)
{
  // Compare the lower endpoints first.
  if (i.min() < j.min())
    return std::strong_ordering::greater;
  if (i.min() > j.min())
    return std::strong_ordering::less;

  // Compare the upper endpoints next.
  if (i.max() > j.max())
    return std::strong_ordering::greater;
  if (i.max() < j.max())
    return std::strong_ordering::less;

  return std::strong_ordering::equal;
}

template<typename T>
bool
includes(const cunits<T> &i, const cunits<T> &j)
{
  return i.min() <= j.min() && j.max() <= i.max();
}

template <typename T>
std::ostream &
operator << (std::ostream &out, const cunits<T> &cu)
{
  out << '{' << cu.min() << ", " << cu.max() << '}';
  return out;
}

template <typename T>
std::istream &
operator >> (std::istream &in, cunits<T> &cu)
{
  char c;
  T min, max;

  // Get the initial '{'.
  in >> c;
  if (!in.fail())
    {
      assert(c == '{');
      // Get the min value.
      in >> min;
      assert(!in.fail());
      // Get the ','.
      in >> c;
      assert(c == ',');
      assert(!in.fail());
      // Get the max value.
      in >> max;
      assert(!in.fail());
      // Get the trailing '}'.
      in >> c;
      assert(c == '}');
      assert(!in.fail());

      if (in)
        {
          // Make sure min < max.
          assert(min < max);

          cu = cunits<T>(min, max);
        }
    }

  return in;
}

#endif // CUNITS_HPP
