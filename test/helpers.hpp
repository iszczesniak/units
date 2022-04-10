#ifndef HELPERS_HPP
#define HELPERS_HPP

// Make sure that i < j.
template <typename T>
bool
is_less(const T &i, const T &j)
{
  bool status = true;

  status &= (i < j);
  status &= (i <= j);
  status &= (i != j);
  status &= !(i == j);
  status &= (!(i > j));
  status &= (!(i >= j));

  return status;
}

// Make sure that i == j.
template <typename T>
bool
is_equal(const T &i, const T &j)
{
  bool status = true;

  status &= (i == j);
  status &= (!(i != j));
  status &= (i <= j);
  status &= (i >= j);
  status &= (!(i < j));
  status &= (!(i > j));

  return status;
}

// Make sure that i > j.
template <typename T>
bool
is_greater(const T &i, const T &j)
{
  return is_less(j, i);
}

#endif // HELPERS_HPP
