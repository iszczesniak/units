#ifndef CUNITS_HPP
#define CUNITS_HPP

#include <cassert>
#include <iostream>
#include <list>

// Describes contiguous units [min, max), i.e., min is included, and
// max is not.

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

  operator bool() const
  {
    return !empty();
  }

  // This operator establishes strict weak ordering needed for sorting
  // in, e.g., containers.  We cannot use the inclusion relation,
  // because it does not induce strick weak ordering as the
  // incomparability of intervals is not transitive.
  //
  // We have to assure that if i properly includes j, then i < j.
  //
  // If an interval is not properly included in another interval, then
  // these intervals are incomparable according to the inclusion
  // relation.  We need to establish strict weak ordering between the
  // incomparable intervals, and we do this by comparing the lower
  // endpoints of the intervals.
  //
  // These are the possibilities of comparing i < j:
  //
  // 1. If min(i) < min(j): always i < j, regardless of:
  //
  //   a. max(i) < max(j): i and j are incomparable, but we say i < j
  //                       because we need to establish a weak strict
  //                       ordering, and the ordering between
  //                       incomparable labels can be arbitrary
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
  // This relation is transitive.
  //
  // This relation does not induce the lexicographic ordering.  It
  // would, had we compared the upper endpoints with <, but then i < j
  // would not hold (while it should) when i includes j.

  bool
  operator < (const cunits &a) const
  {
    return (m_min < a.m_min || m_min == a.m_min && m_max > a.m_max);
  }

  bool
  operator == (const cunits &a) const
  {
    return m_min == a.m_min && m_max == a.m_max;
  }

  bool
  operator != (const cunits &a) const
  {
    return !(*this == a);
  }
};

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
