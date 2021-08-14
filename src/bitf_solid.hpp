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

// number of bits needed to store the value
template <class T>
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
template <class T>
std::string
to_string (T bits)
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
template <class T, class BitT>
T
get_scalar (BitT bits, int index, size_t offset = 1)
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

template <class It, class BitT>
void
get_bulk (It start, It end, BitT bits, size_t offset = 1, int index = 0)
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
set_scalar (BitT bits, int index, size_t offset, T value)
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

// update bitfield with generic container
template <class It, class BitT>
BitT
set_bulk (It start, It end, BitT bits, size_t offset = 1, int index = 0)
{
  __BITF_ASSERT_ITERATOR(It);
  using T = __BITF_VALUE_TYPE_OF(*start);
  __BITF_ASSERT_INTEGRAL (T);
  __BITF_ASSERT_UNSIGNED (BitT);
  
  if (end < start)
  {
    throw std::range_error("end < start");
  }
  size_t n = end - start;
  if (n == 0){
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
      throw std::overflow_error ("cont size > BitT capacity");
    }

  T maxvalue = start[0];
  size_t i = 0;
  while (i < n)
    {
      if (start[i] > maxvalue)
        {
          maxvalue = start[i];
        }
      i++;
    }
  if (bit_size<T> (maxvalue) > offset)
    {
      throw std::overflow_error ("cont value > offset");
    }

  BitT offsetmask
      = max_value<BitT> () >> (bit_capacity<BitT> () - (offset * n + index));
  BitT indexmask = offsetmask >> (offset * n);
  BitT mask = ~(offsetmask ^ indexmask);
  bits &= mask;
  i = 0;
  while (i < n)
    {
      bits |= ((BitT)start[i] << (offset * i + index));
      i++;
    }
  return bits;
}

namespace cls
{
  
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
  to_string () const
  {
    return solid::to_string<T> (_bits);
  };
};

template <class T, class B = T> class constructor : public virtual data<B>
{
public:
  using data<B>::data;
  /**
   * create bitfield with atomic value
   * other bits are set to 0
   */
  constructor (T value, int index, B bits = 0UL)
  {
    data<B>::_bits
        = solid::set_scalar<T> (bits, index, bit_size<T> (value), value);
  };
  /**
   * create bitfield with vector of atomic values
   * other bits are set to 0
   */
  constructor (std::vector<T> values, int index, size_t offset, B bits = 0UL)
  {
    data<B>::_bits = solid::set_bulk(values.begin(), values.end(), (B) bits, offset, index);
  };
  virtual ~constructor () = default;
};

template <class T, class B = T> class accessor : public virtual data<B>
{
public:
  // get atomic value from bitdata
  virtual T
  get_scalar (int index, size_t offset) const
  {
    return solid::get_scalar<T> (data<B>::_bits, index, offset);
  };
  // get vector of n atomic values from bitdata
  virtual std::vector<T>
  get_vector (int index, size_t offset, size_t n) const
  {
    auto vec = std::vector<T> (n);
    solid::get_bulk(vec.begin(), vec.end(), data<B>::_bits, offset, index);
    return vec;
  };
  // get single bit value by index
  virtual T
  operator[] (int index) const
  {
    return solid::get_scalar<T> (data<B>::_bits, index, 1);
  };
};

template <class T, class B = T> class mutator : public virtual data<B>
{
public:
  // set bits value
  virtual void
  set (T value)
  {
    data<B>::_bits = value;
  };

  // insert atomic value to bitfield
  virtual void
  set_scalar (int index, size_t offset, T value)
  {
    data<B>::_bits
        = solid::set_scalar<T> (data<B>::_bits, index, offset, value);
  };

  // insert vector of atomic values to bitfield
  virtual void
  set_vector (int index, size_t offset, std::vector<T> values)
  {
    data<B>::_bits = solid::set_bulk(values.begin(), values.end(), data<B>::_bits,offset, index);
  };
};

} // namespace bitf::solid::cls

} // namespace bitf::solid

} // namespace bitf