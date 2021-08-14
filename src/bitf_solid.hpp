#pragma once
#include <array>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#define __BITF_STRINGIFY(T) #T

#define __BITF_ASSERT_MSG_UNSIGNED(T)                                         \
  __BITF_STRINGIFY (T must be unsigned integer)

#define __BITF_ASSERT_MSG_INTEGRAL(T) __BITF_STRINGIFY (T must be integer)

#define __BITF_ASSERT_MSG_ITERATOR(T)                                         \
  __BITF_STRINGIFY (T must be pointer or iterator)

/**
 * static assertion for unsigned type
 * */
#define __BITF_ASSERT_INTEGRAL(T)                                             \
  static_assert (std::is_integral<T>::value, __BITF_ASSERT_MSG_INTEGRAL (T))

/**
 * static assertion for unsigned type
 * */
#define __BITF_ASSERT_UNSIGNED(T)                                             \
  static_assert (std::is_unsigned<T>::value, __BITF_ASSERT_MSG_UNSIGNED (T))

template<class T> struct __deref_type     { typedef T type; };
template<class T> struct __deref_type<T*> { typedef T type; };
template<class T> struct __deref_type<T&> { typedef T type; };

#define __BITF_VALUE_TYPE_OF(V) typename __deref_type<decltype(V)>::type; 

#define __BITF_ASSERT_ITERATOR(T)                                             \
  static_assert (                                                             \
      std::is_pointer<T>::value                                              \
          || !std::is_same<typename std::iterator_traits<T>::value_type,     \
                           void>::value,                                      \
      __BITF_ASSERT_MSG_ITERATOR(T))

namespace bitf
{
/**
 * bitfields with generic unsigned integer storage
 */
namespace solid
{

// max value of bitfield storage type
template <class T>
inline T
max_value ()
{
  __BITF_ASSERT_UNSIGNED (T);
  return ~0;
}

// number of stored bits
template <class T>
inline size_t
bit_capacity ()
{
  __BITF_ASSERT_INTEGRAL (T);
  return sizeof (T) << 0b11;
}

// max bitfield index with offset = 1
template <class T>
inline size_t
max_index ()
{
  __BITF_ASSERT_INTEGRAL (T);
  return (sizeof (T) << 0b11) - 1;
}

// number of bits needed to store value
template <class T = size_t>
size_t
bit_size (T value)
{
  __BITF_ASSERT_INTEGRAL (T);
  size_t n = 1;
  while (value >>= 1)
    {
      n++;
    }
  return n;
}

// stringify binary representation of bitfield
template <class T = size_t>
std::string
to_str (T bits)
{
  __BITF_ASSERT_UNSIGNED (T);
  size_t size = bit_capacity<T> ();
  std::string res (size, '0');
  for (size_t i = 0; i < size; i++)
    {
      if ((bits >> i) & 1)
        {
          res[size - i - 1] = '1';
        }
    }
  return res;
}

// get atomic value from bitdata
template <class T, class BitT = size_t>
T
get_scalar (BitT bits, int index, size_t offset)
{
  __BITF_ASSERT_UNSIGNED (BitT);
  __BITF_ASSERT_INTEGRAL (T);

  index &= max_index<BitT> ();
  if (offset + index > bit_capacity<BitT> ())
    {
      throw std::overflow_error ("offset + index > BitT capacity");
    }
  if (offset > bit_capacity<T> ())
    {
      throw std::overflow_error ("offset > T capacity");
    }
  BitT offsetmask = max_value<BitT> () >> (bit_capacity<BitT> () - offset);
  return (T)(bits >> index) & offsetmask;
}

// get vector of n atomic values from bitdata
template <class T, class BitT = size_t>
std::vector<T>
get_vector (BitT bits, int index, size_t offset, size_t n)
{
  __BITF_ASSERT_UNSIGNED (BitT);
  __BITF_ASSERT_INTEGRAL (T);

  index &= max_index<BitT> ();

  if (offset * n + index > bit_capacity<BitT> ())
    {
      throw std::overflow_error ("offset + index > BitT capacity");
    }
  if (offset > bit_capacity<T> ())
    {
      throw std::overflow_error ("offset > T capacity");
    }

  size_t maxn = (bit_capacity<BitT> () - index) / offset;
  if (n > maxn)
    {
      throw std::overflow_error ("vector size > BitT capacity");
    }
  std::vector<T> res{};

  BitT offsetmask = max_value<BitT> () >> (bit_capacity<BitT> () - offset);

  for (size_t i = 0; i < n; i++)
    {
      size_t rshift = (index + (offset * i));
      BitT shifted = bits >> rshift;
      BitT val = (shifted & offsetmask);
      res.push_back ((T)val);
    }
  return res;
}

// get array of n atomic values from bitdata
template <class T, size_t N, class BitT = size_t>
std::array<T, N>
get_array (BitT bits, int index, size_t offset)
{
  __BITF_ASSERT_UNSIGNED (BitT);
  __BITF_ASSERT_INTEGRAL (T);

  index &= max_index<BitT> ();

  if (offset * N + index > bit_capacity<BitT> ())
    {
      throw std::overflow_error ("offset + index > BitT capacity");
    }
  if (offset > bit_capacity<T> ())
    {
      throw std::overflow_error ("offset > T capacity");
    }

  size_t maxn = (bit_capacity<BitT> () - index) / offset;
  if (N > maxn)
    {
      throw std::overflow_error ("vector size > BitT capacity");
    }
  std::array<T, N> res{};

  BitT offsetmask = max_value<BitT> () >> (bit_capacity<BitT> () - offset);

  for (size_t i = 0; i < N; i++)
    {
      size_t rshift = (index + (offset * i));
      BitT shifted = bits >> rshift;
      BitT val = (shifted & offsetmask);
      res[i] = val;
    }
  return res;
}

template <class It, class BitT = size_t>
void
collect (It start, It end, BitT bits, size_t offset = 1, int index = 0)
{
  __BITF_ASSERT_ITERATOR(It);
  using T = __BITF_VALUE_TYPE_OF(*start);
  __BITF_ASSERT_INTEGRAL (T);
  __BITF_ASSERT_UNSIGNED (BitT);

  index &= max_index<BitT> ();
  if (offset > bit_capacity<T> ())
    {
      throw std::overflow_error ("offset > T capacity");
    }

  BitT offsetmask = max_value<BitT> () >> (bit_capacity<BitT> () - offset);
  size_t i{ 0 };

  while (start < end)
    {
      size_t rshift = (index + (offset * i++));
      BitT shifted = bits >> rshift;
      BitT val = (shifted & offsetmask);
      *(start++) = (T)val;
    }
}

// insert atomic value to bitfield
template <class T, class BitT>
BitT
insert_scalar (T value, int index, size_t offset, BitT bits = 0)
{
  __BITF_ASSERT_UNSIGNED (BitT);
  __BITF_ASSERT_INTEGRAL (T);

  index &= max_index<BitT> ();
  if (offset + index > bit_capacity<BitT> ())
    {
      throw std::overflow_error ("offset + index > BitT capacity");
    }
  if (bit_size<T> (value) > offset)
    {
      throw std::overflow_error ("value bit width > offset");
    }
  BitT offsetmask
      = max_value<BitT> () >> (bit_capacity<BitT> () - (offset + index));
  BitT indexmask = offsetmask >> offset;
  BitT mask = ~(offsetmask ^ indexmask);
  bits &= mask;
  bits |= (value << index);
  return bits;
}

// insert vector of atomic values to bitfield
template <class T, class BitT = size_t>
BitT
insert_vector (const std::vector<T> &values, int index, size_t offset,
               BitT bits = 0U)
{
  __BITF_ASSERT_UNSIGNED (BitT);
  __BITF_ASSERT_INTEGRAL (T);

  size_t n = values.size ();
  if (!n)
    {
      return bits;
    }

  index &= max_index<BitT> ();
  if (offset * n + index > bit_capacity<BitT> ())
    {
      throw std::overflow_error ("offset + index > BitT capacity");
    }

  size_t maxn = (bit_capacity<BitT> () - index) / offset;
  if (n > maxn)
    {
      throw std::overflow_error ("vector size > BitT capacity");
    }

  T maxvalue = values[0];
  size_t i = 0;
  while (i < n)
    {
      if (values[i] > maxvalue)
        {
          maxvalue = values[i];
        }
      i++;
    }
  if (bit_size<T> (maxvalue) > offset)
    {
      throw std::overflow_error ("vector value > offset");
    }

  BitT offsetmask
      = max_value<BitT> () >> (bit_capacity<BitT> () - (offset * n + index));
  BitT indexmask = offsetmask >> (offset * n);
  BitT mask = ~(offsetmask ^ indexmask);
  bits &= mask;
  i = 0;
  while (i < n)
    {
      bits |= ((BitT)values[i] << (offset * i + index));
      i++;
    }
  return bits;
}

// insert vector of atomic values to bitfield
template <class T, size_t N, class BitT = size_t>
BitT
insert_array (const std::array<T, N> &values, int index, size_t offset,
              BitT bits = 0)
{

  __BITF_ASSERT_UNSIGNED (BitT);
  __BITF_ASSERT_INTEGRAL (T);
  if (0 == N)
    {
      return bits;
    }

  index &= max_index<BitT> ();
  if (offset * N + index > bit_capacity<BitT> ())
    {
      throw std::overflow_error ("offset + index > BitT capacity");
    }

  size_t maxn = (bit_capacity<BitT> () - index) / offset;
  if (N > maxn)
    {
      throw std::overflow_error ("vector size > BitT capacity");
    }

  T maxvalue = values[0];
  size_t i = 0;
  while (i < N)
    {
      if (values[i] > maxvalue)
        {
          maxvalue = values[i];
        }
      i++;
    }
  if (bit_size<T> (maxvalue) > offset)
    {
      throw std::overflow_error ("vector value > offset");
    }

  BitT offsetmask
      = max_value<BitT> () >> (bit_capacity<BitT> () - (offset * N + index));
  BitT indexmask = offsetmask >> (offset * N);
  BitT mask = ~(offsetmask ^ indexmask);
  bits &= mask;
  i = 0;
  while (i < N)
    {
      bits |= (values[i] << (offset * i + index));
      i++;
    }
  return bits;
}

template <class T> class data
{
protected:
  __BITF_ASSERT_UNSIGNED (T);
  T _bits;

public:
  data () : _bits (0){};
  explicit data (T val) : _bits (val){};
  data (data const &bd) : _bits (bd._bits){};
  data (data &&bd) noexcept : _bits (bd._bits){};

  virtual ~data () = default;

  data<T> &
  operator= (data<T> const &other)
  {
    _bits = other._bits;
    return *this;
  }
  data<T> &
  operator= (data<T> &&other) noexcept
  {
    _bits = other._bits;
    return *this;
  }

  bool
  operator== (const data<T> &other) const
  {
    return _bits == other._bits;
  }
  bool
  operator!= (const data<T> &other) const
  {
    return _bits != other._bits;
  }

  // full bitfield value
  T
  bits () const
  {
    return _bits;
  };

  // stringify binary representation of bitfield
  virtual std::string
  to_str () const
  {
    return solid::to_str<T> (_bits);
  };
};

template <class T> class constructor : public virtual data<T>
{
public:
  using data<T>::data;
  /**
   * create bitfield with atomic value
   * other bits are set to 0
   */
  constructor (T value, int index, T bits = 0)
  {
    data<T>::_bits
        = solid::insert_scalar<T> (value, index, bit_size<T> (value), bits);
  };
  /**
   * create bitfield with vector of atomic values
   * other bits are set to 0
   */
  constructor (std::vector<T> values, int index, size_t offset, T bits = 0)
  {
    data<T>::_bits = solid::insert_vector<T> (values, index, offset, bits);
  };
  virtual ~constructor () = default;
};

template <class T> class accessor : public virtual data<T>
{
public:
  // get atomic value from bitdata
  virtual T
  get_scalar (int index, size_t offset) const
  {
    return solid::get_scalar<T> (data<T>::_bits, index, offset);
  };
  // get vector of n atomic values from bitdata
  virtual std::vector<T>
  get_vector (int index, size_t offset, T n) const
  {
    return solid::get_vector<T> (data<T>::_bits, index, offset, n);
  };
  // get single bit value by index
  virtual T
  operator[] (int index) const
  {
    return solid::get_scalar<T> (data<T>::_bits, index, 1);
  };
};

template <class T> class mutator : public virtual data<T>
{
public:
  // set bits value
  virtual void
  set (T value)
  {
    data<T>::_bits = value;
  };

  // insert atomic value to bitfield
  virtual void
  insert_scalar (T value, int index, size_t offset)
  {
    data<T>::_bits
        = solid::insert_scalar<T> (value, index, offset, data<T>::_bits);
  };

  // insert vector of atomic values to bitfield
  virtual void
  insert_vector (std::vector<T> values, int index, size_t offset)
  {
    data<T>::_bits
        = solid::insert_vector<T> (values, index, offset, data<T>::_bits);
  };
};

} // namespace bitf::solid

} // namespace bitf