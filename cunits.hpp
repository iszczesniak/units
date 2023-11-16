#ifndef CUNITS_HPP
#define CUNITS_HPP

#include <cassert>
#include <compare>
#include <iostream>
#include <list>

// Describes a resource interval [min, max), i.e., min is included,
// and max is not.

// Relations between resources i and j, where:
//
// * || is incomparable,
//
// * sp is a proper superset,
//
// * sb is a proper subset.
//
//                 -------------------------------------------------------
//                 | max(i) < max(j) | max(i) = max(j) | max(i) > max(j) |
//                 -------------------------------------------------------
// min(i) < min(j) | i || j | i < j  | i sp j | i < j  | i sp j | i < j  |
//                 -------------------------------------------------------
// min(i) = min(j) | i sb j | i > j  |      i = j      | i sp j | i < j  |
//                 -------------------------------------------------------
// min(i) > min(j) | i sb j | i > j  | i sb j | i > j  | i || j | i > j  |
//                 -------------------------------------------------------

template <typename T>
class cunits
{
  // lower and upper endpoints
  T m_min, m_max;

public:
  cunits(): m_min(0), m_max(0)
  {
  }

  cunits(T min, T max): m_min(min), m_max(max)
  {
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

  bool operator == (const cunits &) const = default;
};

// The < operator establishes an ordering needed for sorting in, e.g.,
// containers.  We need <, because we cannot use the inclusion
// relation: the inclusion relation is not a strick weak ordering as
// the incomparability is intransitive.
//
// We define i < j like this (the same as in the table at the top):
//
// 1. If min(i) < min(j): always i < j, regardless of:
//
//   a. max(i) < max(j): i and j are incomparable, and so we need to
//                       break this tie somehow; the ordering between
//                       incomparable labels can be arbitrary, but we
//                       say i < j;
//
//   b. max(i) == max(j): i < j because i properly includes j
//
//   c. max(i) > max(j): i < j because i properly includes j
//
// 2. If min(i) == min(j), and:
//
//   a. max(i) < max(j): j properly includes i, and so j < i
//
//   b. max(i) == max(j): i == j, and so !(i < j)
//
//   c. max(i) > max(j): j < i, same as 2a, just swap i and j
//
// 3. If min(i) > min(j), and:
//
//   a. max(i) < max(j): same as 1c, just swap i and j
//
//   b. max(i) == max(j): same as 1b, just swap i and j
//
//   c. max(i) > max(j): same as 1a, just swap i and j
//
// Ordering < is transitive.
//
// To wrap up:
//
// * i < j if:
//
//   min(i) < min(j) || min(i) == min(j) && max(i) > max(j)
//
// * i > j if:
//
//   min(i) > min(j) || min(i) == min(j) && max(i) < max(j)
//
// * i == j otherwise.

template<typename T>
constexpr auto
operator <=> (const cunits<T> &i, const cunits<T> &j)
{
  if (i.min() < j.min())
    return std::strong_ordering::less;
  if (i.min() > j.min())
    return std::strong_ordering::greater;
  if (i.max() > j.max())
    return std::strong_ordering::less;
  if (i.max() < j.max())
    return std::strong_ordering::greater;

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

template <typename T>
auto
get_candidate_intervals(const cunits<T> &cu, int ncu)
{
  std::list<cunits<T>> result;

  if (ncu <= cu.size())
    for(auto m = cu.min(); m <= cu.max() - ncu; ++m)
      result.push_back(cunits<T>(m, m + ncu));

  return result;
}

#endif // CUNITS_HPP
