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

template <typename T>
struct data_type_cmp
{
  bool
  operator()(const cunits<T> &a, const cunits<T> &b)
  {
    return a.max() <= b.min();
  }
};

// The set of units.
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
  using base_type::const_iterator;

  // We delegate <=> to base_type.  We do not want the default
  // implementation of <=> (i.e., lexicographic comparison), because
  // m_edge should not take part.
  constexpr auto operator <=> (const sunits &l) const = default;

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
    auto min = cu.min();
    auto max = cu.max();
    auto i = std::upper_bound(begin(), end(), cu, data_type_cmp<T>());
    auto j = i;

    // We may need to remove a neighboring CUs.
    
    // We look for a neighboring CU on the left.
    if (auto i2 = i; i2 != begin() && (--i2)->max() == cu.min())
      {
        --i;
        min = i->min();
      }

    // We look for a neighboring CU on the right.
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

  // Remove a CU.
  void
  remove(const data_type &cu)
  {
    // We must find a CU from which we remove the given cu.
    auto i = std::lower_bound(begin(), end(), cu, data_type_cmp<T>());
    // There must be a CU, and so we don't expect to reach the end.
    assert(i != end());

    // The cunits to be removed.
    const auto rcu = *i;
    assert(rcu.includes(cu));
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

  // Remove those cunits with less than ncu units.
  void
  remove(size_type ncu)
  {
    base_type::erase(std::remove_if(begin(), end(),
                                    [ncu](const auto &cu)
                                    {return cu.size() < ncu;}),
                     end());
  }

  bool
  operator == (const sunits &a) const
  {
    if (size() != a.size())
      return false;

    for (auto i = begin(), j = a.begin(); i != end(); ++i, ++j)
      if (*i != *j)
        return false;

    return true;
  }

  bool
  operator != (const sunits &a) const
  {
    return !(*this == a);
  }

private:
  // Make sure the cunits are in the right order.
  bool
  verify()
  {
    if (auto i = begin(); i != end())
      for (auto p = i; ++i != end(); ++p)
        if (p->max() > i->min())
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
  auto i = std::upper_bound(su.begin(), su.end(), cu,
                            data_type_cmp<T>());
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

template <typename T>
auto
get_candidate_slots(const sunits<T> &su, int ncu)
{
  std::list<cunits<T>> result;

  for (const auto &cu: su)
    {
      auto r = get_candidate_slots(cu, ncu);
      result.insert(result.end(), r.begin(), r.end());
    }

  return result;
}

#endif // SUNITS_HPP
