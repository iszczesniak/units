#ifndef SUNITS_HPP
#define SUNITS_HPP

#include "cunits.hpp"

#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <iostream>
#include <list>
#include <numeric>
#include <vector>

// The set of cunits (CU).  The base container stores non-overlapping
// CUs sorted with < rewritten from <=>.
//
// Three functions below search through the base container using
// std::upper_bound:
//
// * insert 
//
// * remove
//
// * includes
//
// That searching takes a CU and finds the first CU in the base
// container such that: 

template <typename T>
class sunits: private std::vector<cunits<T>>
{
  using data_type = cunits<T>;
  using base_type = std::vector<data_type>;
  using size_type = T;

public:
  sunits()
  {
  }

  sunits(std::initializer_list<data_type> l)
  {
    for (const auto &cu: l)
      insert(cu);
  }

  using base_type::begin;
  using base_type::end;
  using base_type::size;
  using base_type::empty;

  //  constexpr bool operator == (const sunits &) const = default;
  //  constexpr auto operator <=> (const sunits &l) const = default;

  auto
  size() const
  {
    return std::accumulate(begin(), end(), 0,
                           [](auto c, const auto &cu)
                           {return c + cu.size();});
  }

  // Insert a CU.
  void
  insert(const data_type &cu)
  {
    auto i = std::upper_bound(begin(), end(), cu);
    auto j = i;

    // These are the endpoints of the new CU.  Look left and right for
    // neighboring CUs to remove and then to merge into the new CU.
    auto min = cu.min();
    auto max = cu.max();

    // Left.
    if (auto i2 = i; i2 != begin() && (--i2)->max() == cu.min())
      {
        --i;
        min = i->min();
      }

    // Right.
    if (j != end() && cu.max() == j->min())
      {
        max = j->max();
        ++j;
      }

    j = base_type::erase(i, j);
    data_type icu(min, max);
    auto pos = base_type::insert(j, icu);
    // Make sure the insertion was successfull.
    assert(*pos == icu);

    assert(verify());
  }

  // Remove a CU.  Object *this must include the cu.
  void
  remove(const data_type &cu)
  {
    // We must find a CU from which we remove the given cu.  Iterator
    // i points to the first element for which cu < i*.
    auto i = std::upper_bound(begin(), end(), cu);

    // The element previous to *i must be less than or equal to cu.
    assert(i != begin());
    --i;

    // The cunits to be removed.
    const auto rcu = *i;
    assert(includes(rcu, cu));
    // We have to remove the CU we found.
    i = base_type::erase(i);

    // If there were some units on the right in the removed CU, we
    // have to add them.
    if (cu.max() < rcu.max())
      i = base_type::insert(i, data_type(cu.max(), rcu.max()));

    // If there were some units on the left in the removed CU, we have
    // to add them.
    if (rcu.min() < cu.min())
      base_type::insert(i, data_type(rcu.min(), cu.min()));

    assert(verify());
  }

private:
  // Make sure the objects are in the order.
  bool
  verify()
  {
    // Make sure the container is not empty.
    if (auto i = begin(); i != end())
      // Iterate over every neighbouring pair: p is previous to i.
      for (auto p = i; ++i != end(); ++p)
        // Must hold: p->max() < i->min().  They cannot equal, because
        // then they should have been merged.
        if (!(p->max() < i->min()))
          return false;

    return true;
  }
};

template <typename T>
std::ostream &
operator << (std::ostream &out, const sunits<T> &su)
{
  out << '{';

  bool first = true;

  for (const auto &cu: su)
    {
      if (!first)
        out << ", ";
      out << cu;
      first = false;
    }

  out << '}';

  return out;
}

template <typename T>
std::istream &
operator >> (std::istream &in, sunits<T> &su)
{
  char c;

  in >> c;
  assert (!in.fail() && c == '{');

  for (cunits<T> cu; in >> cu;)
    {
      su.insert(cu);

      in >> c;

      if (c == '}')
        break;
    }

  return in;
}

template <typename T>
bool
includes(const sunits<T> &a, const sunits<T> &b)
{
  auto i = a.begin();

  // Every cu of a, has to be in *this.
  for(const auto &cu: b)
    while(true)
      {
        if (i == a.end())
          return false;

        if (includes(*i, cu))
          break;

        if (cu.min() < i->min())
          return false;

        ++i;
      }

  return true;
}

template <typename T>
bool
includes(const sunits<T> &su, const cunits<T> &cu)
{
  auto i = std::upper_bound(su.begin(), su.end(), cu);
  return i != su.begin() && includes(*(--i), cu);
}

template <typename T>
sunits<T>
intersection(const sunits<T> &a, const sunits<T> &b)
{
  sunits<T> ret;

  auto i = a.begin(), j = b.begin();
  while(i != a.end() && j != b.end())
    {
      if (i->max() <= j->min())
        {
          ++i;
          continue;
        }

      if (j->max() <= i->min())
        {
          ++j;
          continue;
        }

      // At this point the CU's of i and j overlap.
      auto min = std::max(i->min(), j->min());
      auto max = std::min(i->max(), j->max());
      ret.insert({min, max});

      i->max() < j->max() ? ++i : ++j;
    }

  return ret;
}

template <typename T>
sunits<T>
intersection(const cunits<T> &a, const sunits<T> &b)
{
  return intersection(sunits<T>{a}, b);
}

#endif // SUNITS_HPP
