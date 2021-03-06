#ifndef CUNITS_HPP
#define CUNITS_HPP

#include <cassert>
#include <compare>
#include <iostream>
#include <list>

// Describes contiguous units [min, max), i.e., min is included, and
// max is not.

template <typename T>
class cunits
{
  // I need to keep these fields private, so that they are not
  // mistakenly modified outside of class.  I can't make these fields
  // const, because STL containers require the element type be
  // assignable, and const members cannot be assigned.
  T m_min;
  T m_max;

public:
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
  count() const
  {
    return m_max - m_min;
  }

  bool
  empty() const
  {
    return !count();
  }

  operator bool() const
  {
    return !empty();
  }

  // We establish strong ordering with the default <=> operator.
  std::strong_ordering
  operator <=> (const cunits &a) const = default;

  friend bool
  includes<>(const cunits &, const cunits &);
};

template <typename T>
T
min(const cunits<T> &a)
{
  return a.min();
}

template <typename T>
T
max(const cunits<T> &a)
{
  return a.max();
}

template <typename T>
bool
includes(const cunits<T> &a, const cunits<T> &b)
{
  return a.m_min <= b.m_min && b.m_max <= a.m_max;
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
get_candidate_slots(const cunits<T> &cu, int ncu)
{
  std::list<cunits<T>> result;

  if (ncu <= cu.count())
    for(auto m = cu.min(); m <= cu.max() - ncu; ++m)
      result.push_back(cunits<T>(m, m + ncu));

  return result;
}

#endif // CUNITS_HPP
