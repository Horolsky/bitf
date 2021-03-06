#pragma once
#include <array>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#define _BITF_STRINGIFY(T) #T

#define _BITF_ASSERT_MSG_UNSIGNED(T)                                          \
  _BITF_STRINGIFY (T must be unsigned integer)

#define _BITF_ASSERT_MSG_INTEGRAL(T) _BITF_STRINGIFY (T must be integer)

#define _BITF_ASSERT_MSG_ITERATOR(T)                                          \
  _BITF_STRINGIFY (T must be pointer or iterator)

/**
 * static assertion for unsigned type
 * */
#define _BITF_ASSERT_INTEGRAL(T)                                              \
  static_assert (std::is_integral<T>::value, _BITF_ASSERT_MSG_INTEGRAL (T))

/**
 * static assertion for unsigned type
 * */
#define _BITF_ASSERT_UNSIGNED(T)                                              \
  static_assert (std::is_unsigned<T>::value, _BITF_ASSERT_MSG_UNSIGNED (T))

template<class T> struct _bitf_deref_type     { typedef T type; };
template<class T> struct _bitf_deref_type<T*> { typedef T type; };
template<class T> struct _bitf_deref_type<T&> { typedef T type; };

#define _BITF_VALUE_TYPE_OF(V) typename _bitf_deref_type<decltype (V)>::type;

#define _BITF_ASSERT_ITERATOR(T)                                              \
  static_assert (                                                             \
      std::is_pointer<T>::value                                               \
          || !std::is_same<typename std::iterator_traits<T>::value_type,      \
                           void>::value,                                      \
      _BITF_ASSERT_MSG_ITERATOR (T))

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
  _BITF_ASSERT_UNSIGNED (T);
  return ~0;
}

// number of stored bits
template <class T>
inline size_t
bit_capacity ()
{
  _BITF_ASSERT_INTEGRAL (T);
  return sizeof (T) << 0b11;
}

// max bitfield index with offset = 1
template <class T>
inline size_t
max_index ()
{
  _BITF_ASSERT_INTEGRAL (T);
  return (sizeof (T) << 0b11) - 1;
}

// number of bits needed to store the value
template <class T>
size_t
bit_size (T value)
{
  _BITF_ASSERT_INTEGRAL (T);
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
  _BITF_ASSERT_UNSIGNED (T);
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
get_scalar (BitT bits, size_t offset, size_t indent) noexcept
{
  _BITF_ASSERT_UNSIGNED (BitT);
  _BITF_ASSERT_INTEGRAL (T);

  BitT offsetmask = ~(max_value<BitT>() << offset);
  return (T)(bits >> indent) & offsetmask;
}

// insert atomic value to bitfield
template <class T, class BitT>
BitT
set_scalar (T value, BitT bits, size_t offset, size_t indent) noexcept
{
  _BITF_ASSERT_UNSIGNED (BitT);
  _BITF_ASSERT_INTEGRAL (T);

  BitT lmask = max_value<BitT>() << (offset+indent);
  BitT rmask = ~(max_value<BitT>() << indent);
  bits &= (lmask^rmask);
  BitT offsetmask = ~(max_value<BitT>() << (offset));
  bits |= ((BitT) value & offsetmask) << indent;
  return bits;
}

template <class It, class BitT>
void
get_bulk (It start, It end, BitT bits, size_t offset = 1, size_t indent = 0) noexcept
{
  _BITF_ASSERT_ITERATOR (It);
  using T = _BITF_VALUE_TYPE_OF (*start);
  _BITF_ASSERT_INTEGRAL (T);
  _BITF_ASSERT_UNSIGNED (BitT);

  BitT new_bits = bits & ~(max_value<BitT>() << indent);
  BitT offsetmask = ~(max_value<BitT>() << offset);
  while (start < end)
    {
      BitT val = (bits >> indent & offsetmask);
      *(start++) = (T)val;
      indent += offset;
    }
}

// update bitfield with generic container
template <class It, class BitT>
BitT
set_bulk (It start, It end, BitT bits, size_t offset = 1, size_t indent = 0) noexcept
{
  _BITF_ASSERT_ITERATOR (It);
  using T = _BITF_VALUE_TYPE_OF (*start);
  _BITF_ASSERT_INTEGRAL (T);
  _BITF_ASSERT_UNSIGNED (BitT);
  
  BitT new_bits = bits & ~(max_value<BitT>() << indent);
  BitT offsetmask = ~(max_value<BitT>() << (offset));
  while(start != end)
  {
    new_bits |= ((BitT)*start & offsetmask) << indent;
    indent += offset;
    ++start;
  }
  new_bits &= ~(max_value<BitT>()  << indent);
  new_bits |= bits & (max_value<BitT>()  << indent);
  return new_bits;
}

namespace cls
{

template <class T> class data
{
protected:
  _BITF_ASSERT_UNSIGNED (T);
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
  constructor (T value, int indent, B bits = 0UL)
  {
    data<B>::_bits
        = solid::set_scalar<T> (value, bits, bit_size<T> (value), indent);
  };
  /**
   * create bitfield with vector of atomic values
   * other bits are set to 0
   */
  constructor (std::vector<T> values, int indent, size_t offset, B bits = 0UL)
  {
    data<B>::_bits = solid::set_bulk (values.begin (), values.end (), (B)bits,
                                      offset, indent);
  };
  virtual ~constructor () = default;
};

template <class T, class B = T> class accessor : public virtual data<B>
{
public:
  // get atomic value from bitdata
  virtual T
  get_scalar (size_t offset, int indent) const
  {
    return solid::get_scalar<T> (data<B>::_bits, offset, indent);
  };
  // get vector of n atomic values from bitdata
  virtual std::vector<T>
  get_vector (size_t n, size_t offset, int indent = 0) const
  {
    auto vec = std::vector<T> (n);
    solid::get_bulk (vec.begin (), vec.end (), data<B>::_bits, offset, indent);
    return vec;
  };
  // get single bit value by index
  virtual T
  operator[] (int index) const
  {
    return solid::get_scalar<T> (data<B>::_bits, 1UL, index);
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
  set_scalar (T value, size_t offset, int indent)
  {
    data<B>::_bits
        = solid::set_scalar<T> (value, data<B>::_bits, offset, indent);
  };

  // insert vector of atomic values to bitfield
  virtual void
  set_vector (std::vector<T> values, size_t offset, int indent = 0)
  {
    data<B>::_bits = solid::set_bulk (values.begin (), values.end (),
                                      data<B>::_bits, offset, indent);
  };
};

} // namespace bitf::solid::cls

} // namespace bitf::solid

} // namespace bitf