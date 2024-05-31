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

// A sequence of non-overlapping intervals.  Intervals are stored in a
// base container that we keep sorted.  Since the intervals do not
// overlap (and so do not include one another), then < establishes
// linear ordering.  It is enough to compare the intervals by the
// lower endpoints only (and that is covered by the cases 1a and 3c,
// and implemented by <=>).
//
// To keep the implementation simple and efficient, we offer only the
// minimal needed functionality:
//
// * insert only an interval that in no part is already included,
//
// * remove only an interval that is already included.
//
// To insert or remove an interval iv, we need to find the first
// interval in the sequence that is greater than iv (that follows iv).
// Function upper_bound(begin(), end(), iv) returns iterator i to the
// first (as the iterator gets incremented, i.e., from left to right)
// interval in the base container such that iv < *i.  The function may
// return a pointer to the beginning or the end.

template <typename T>
struct sunits: private std::vector<cunits<T>>
{
  using data_type = cunits<T>;
  using base_type = std::vector<data_type>;
  using size_type = T;

  sunits()
  {
  }

  sunits(std::initializer_list<data_type> l)
  {
    for (const auto &cu: l)
      insert(cu);
  }

  constexpr bool operator == (const sunits &) const = default;
  // constexpr auto operator <=> (const sunits &) const = default;

  using base_type::begin;
  using base_type::end;
  using base_type::size;
  using base_type::empty;

  auto
  size() const
  {
    return std::accumulate(begin(), end(), 0,
                           [](auto c, const auto &in)
                           {return c + in.size();});
  }

  // Insert an interval iv.  No part of it can already be included.
  void
  insert(const data_type &iv)
  {
    // Returns a position i where to insert iv.
    //
    // Returned i is such that iv < *i, and since the intervals (iv
    // and in the container) do not overlap, then we put iv just
    // before *i.  If there is an interval p in the container that
    // preceds *i, then iv should be placed afert p because p < iv
    // holds:
    //
    // * iv < p is false (because *i is the first from the left such
    //   that iv < *i), so iv >= p holds (because < is total),
    //
    // * the intervals are totally ordered as they do not overlap,
    //
    // * the intervals in the container are sorted with <.
    //
    // That's how it looks on the axis:
    //
    //      p           iv       *i
    // |----*======o----*===o----*======o------->
    auto i = std::upper_bound(begin(), end(), iv,
                              std::greater<data_type>());
    auto j = i;

    // These are the endpoints of the interval to insert.  Look left
    // and right for neighboring intervals to remove and then to merge
    // into the new interval.
    auto min = iv.min();
    auto max = iv.max();

    // Look left.  By one position only, hence just "if", and not
    // "while".  We need variable i2, because we need to decrement the
    // iterator to look left.  Instead of copying i and (--i2), we
    // could do (i - 1), but that would require a random access
    // iterator.  With --i2, we require only a bidirectional iterator.
    if (i != begin())
      if (auto i2 = i; (--i2)->max() == min)
        --i, min = i->min();

    // Right.
    if (j != end() && max == j->min())
      max = j->max(), ++j;

    j = base_type::erase(i, j);
    data_type icu(min, max);
    auto pos = base_type::insert(j, icu);
    // Make sure the insertion was successfull.
    assert(*pos == icu);

    assert(verify());
  }

  // Remove an interval iv.  The interval must be already included.
  //
  // The container must have at least one interval p:
  //
  // |---*=======o------->
  //
  // These are the possible intervals iv:
  //
  //     *===o       p < vi
  //     *=======*   p == vi
  //         *===o   p < vi
  //      *=====*    p < vi
  //
  // Since p must include or equal vi, then p <= iv.
  //
  // Here's the plan:
  //
  // * find an interval pointed with i such that iv < *i,
  //
  // * there must be a interval p that precedes *i,
  //
  // * interval p includes iv, so work with p.
  void
  remove(const data_type &iv)
  {
    // Iterator i points to the first element for which iv < *i.
    auto i = std::upper_bound(begin(), end(), iv,
                              std::greater<data_type>());

    // There must exist an element p previous to *i such that p <= iv.
    //
    // We know that:
    //
    // * p <= iv - since p includes iv,
    //
    // * interval *i is the first from left for which iv < *i.
    //
    // And so: p <= iv < *i
    assert(i != begin());
    --i;

    // A copy of p that we remove next.
    const auto cop = *i;
    assert(includes(cop, iv));
    // Remove p.
    i = base_type::erase(i);

    // If there were some units on the right in p, we add them.  We
    // first insert the leftover interval from the right side to get
    // iterator i where we insert the leftover interval from the left
    // side.  We can't swap the order (i.e., first from the left, then
    // from the right) because the leftover intervals would be
    // reversed, making the base container inconsistent.
    if (iv.max() < cop.max())
      i = base_type::insert(i, data_type(iv.max(), cop.max()));
    // If there were some units on the left in p, we add them.
    if (cop.min() < iv.min())
      base_type::insert(i, data_type(cop.min(), iv.min()));

    assert(verify());
  }

private:
  // Make sure the intervals are in order.
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

// We have to sort lexicographically ourselves, because the default
// implementation of <=> is not available on OpenBSD yet.  Take a look
// at the commented out defaulted declaration of member <=> above --
// if that finally complies, we can remove the function below.
template <typename T>
auto operator <=> (const sunits<T> &i, const sunits<T> &j)
{
  // Could be as easy as below, but ain't accepted by older compilers.
  //
  // return std::lexicographical_compare_three_way(i.begin(), i.end(),
  //                                               j.begin(),
  //                                               j.end());

  auto ii = i.begin();
  auto ji = j.begin();

  for(; ii != i.end() && ji != j.end(); ++ii, ++ji)
    if (*ii == *ji)
      continue;
    else
      return *ii <=> *ji;

  if (ii == i.end() && ji != j.end())
    return std::strong_ordering::less;

  if (ii != i.end() && ji == j.end())
    return std::strong_ordering::greater;

  return std::strong_ordering::equal;
}

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

// Every interval of b has to be in a.
template <typename T>
bool
includes(const sunits<T> &a, const sunits<T> &b)
{
  auto i = a.begin();

  // Every cu of a, has to be in *this.
  for(const auto &cu: b)
    while(true)
      {
        // We've reached the end of a, but cu was nowhere to be found.
        if (i == a.end())
          return false;

        // If *i includes cu, then *i may include the next cu.
        if (includes(*i, cu))
          break;

        // If cu > *i, then cu either precedes or includes *i, and so
        // cu cannot be included in a.
        if (cu > *i)
          return false;

        ++i;
      }

  return true;
}

// Every interval of b has to be in a. That's another implementation
// that turned out to be slower (in some of my tests) than the above.
template <typename T>
bool
includes2(const sunits<T> &a, const sunits<T> &b)
{
  auto j = b.begin();

  if (j != b.end())
    {
      auto i = std::upper_bound(a.begin(), a.end(), *j,
                                std::greater<cunits<T>>());

      // Only the first search can return i == a.begin();
      if (i == a.begin())
        return false;

      while(true)
        {
          // If *--i2 > *j, then *--i2 either precedes or includes *j,
          // and so *--i2 cannot be included in a.
          if (auto i2 = i; *--i2 > *j)
            return false;

          if (++j == b.end())
            break;

          i = std::upper_bound(i, a.end(), *j,
                               std::greater<cunits<T>>());
        }
    }

  return true;
}

template <typename T>
bool
includes(const sunits<T> &su, const cunits<T> &iv)
{
  using data_type = typename sunits<T>::data_type;

  auto i = std::upper_bound(su.begin(), su.end(), iv,
                            std::greater<data_type>());

  // If there is no preceding interval, then iv is not included.  If
  // there is a preceding interval, then it's the only interval that
  // can include iv.  For reasoning, please read the comments in
  // function "remove".
  return i != su.begin() && includes(*--i, iv);
}

template <typename T>
sunits<T>
intersection(const sunits<T> &a, const sunits<T> &b)
{
  sunits<T> ret;

  auto i = a.begin();
  auto j = b.begin();

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

      // At this point the intervals of i and j overlap.
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
