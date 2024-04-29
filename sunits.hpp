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

// sunits is the set of cunits.  Non-overlapping cunits are stored
// sorted in the base container.
//
//
// We use function upper_bound(begin(), end(), cu) that returns
// iterator i to the first (as the iterator gets incremented, i.e.,
// from left to right) cunits object in the base container such that
// cu < *i.
//
// If there is a cunits p that is on the left of *i (i.e., *(i - 1)
// exists), relation p <= cu must hold because:
//
// * element *i is the first for which cu < *i holds,
//
// * elements in the base container are sorted with <,
//
// * ordering < is transitive and total.
//
// Function std::upper_bound(begin(), end(), cu) may return a pointer
// to the beginning or the end, depending on whether there are such *i
// and p.
//
// As given in the table in cunits.hpp, relation p <= cu holds when:
//
// * p == cu - cunits are equal,
//
// * p < cu - p || cu or p properly includes cu.
//
// The above p || cu of p < cu happens when min(p) < min(cu) and
// max(p) < max(cu) in two cases:
//
// * p precedes cu, i.e., p.max() <= cu.min(),
//
// * p overlaps with cu.

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

  auto operator <=> (const sunits &) const = default;

  using base_type::begin;
  using base_type::end;
  using base_type::size;
  using base_type::empty;

  auto
  size() const
  {
    return std::accumulate(begin(), end(), 0,
                           [](auto c, const auto &cu)
                           {return c + cu.size();});
  }

  // Insert a CU.  Object *this must not include nor overlap with cu.
  void
  insert(const data_type &cu)
  {
    // Returns a position where to insert the cu.  Since the cunits
    // (the cu and those in the base container) do not overlap with
    // and do not include other cunits, then p <= cu < *i means that p
    // precedes cu (p and cu cannot equal) and cu precedes *i.
    auto i = std::upper_bound(begin(), end(), cu);
    auto j = i;

    // These are the endpoints of the new CU.  Look left and right for
    // neighboring CUs to remove and then to merge into the new CU.
    auto min = cu.min();
    auto max = cu.max();

    // Left.  We need variable i2, because we need to decrement the
    // iterator.  Instead of (--i2), we could do (i2 - 1), but that
    // would require a random access iterator.  With --i2, we require
    // only bidirectional iterator.
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
    // Iterator i points to the first element for which cu < i*.
    auto i = std::upper_bound(begin(), end(), cu);

    // There must exist element p previous to *i such that p <= cu.
    assert(i != begin());
    --i;

    // A copy of p that we remove next.
    const auto cop = *i;
    assert(includes(cop, cu));
    // Remove p.
    i = base_type::erase(i);

    // If there were some units on the right in p, we add them.  We
    // first insert the CU from the right side to get iterator i where
    // we insert the CU from the left side.  We can't swap the order
    // (i.e., first from the left, then from the right) because the
    // CUs would be reversed, making the base container inconsistent.
    if (cu.max() < cop.max())
      i = base_type::insert(i, data_type(cu.max(), cop.max()));
    // If there were some units on the left in p, we add them.
    if (cop.min() < cu.min())
      base_type::insert(i, data_type(cop.min(), cu.min()));

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
  // If there is no preceding p, then cu is not included.
  auto i = std::upper_bound(su.begin(), su.end(), cu);
  // If there is a preceding cunits p, then:
  //
  // * p includes cu properly or not - return true,
  //
  // * p precedes cu or overlaps with it - return false.
  return i != su.begin() && includes(*--i, cu);
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
