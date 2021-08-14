#pragma once
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

/**
 * single parameter template: 
 * T: unsigned integer type
 * */
#define __BITF_UINT_T(T)                                            \
  template <typename T,                                                       \
            std::enable_if_t<std::is_unsigned<T>::value, bool> = true>
/**
 * double parameter template: 
 * T: unsigned integer type
 * S: unsigned integer type (def = T)
 * */
#define __BITF_UINT_TT(T, S)                                      \
  template <typename T, typename S = T,                                         \
            std::enable_if_t<std::is_unsigned<T>::value, bool> = true,       \
            std::enable_if_t<std::is_unsigned<S>::value, bool> = true>
/**
 * array building template: 
 * T: unsigned integer type
 * S: unsigned integer type (def = T)
 * N: array size
 * */
#define __BITF_UINT_TTA(T, S, N)                                      \
  template <typename T, typename S = T1, size_t N,                                         \
            std::enable_if_t<std::is_unsigned<T1>::value, bool> = true,       \
            std::enable_if_t<std::is_unsigned<T2>::value, bool> = true>

namespace bitf
{
/**
 * bitfields with generic unsigned integer storage
 */
namespace solid
{

__BITF_UINT_T (T)
// max value of bitfield storage type
inline T
max_value ()
{
  return ~0;
}

__BITF_UINT_T (T)
// number of stored bits
inline size_t
bit_capacity ()
{
  return sizeof (T) << 0b11;
}

__BITF_UINT_T (T)
// max bitfield index with offset = 1
inline size_t
max_index ()
{
  return (sizeof (T) << 0b11) - 1;
}

__BITF_UINT_T (T)
// number of bits needed to store value
size_t
bit_size (T value)
{
  size_t n = 1;
  while (value >>= 1)
    {
      n++;
    }
  return n;
}

__BITF_UINT_T (T)
// stringify binary representation of bitfield
std::string
to_str (T bits)
{
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

__BITF_UINT_TT (BitT, RetT)
// get atomic value from bitdata
RetT
get_scalar (BitT bits, int index, size_t offset)
{
  index &= max_index<BitT> ();
  if (offset + index > bit_capacity<BitT> ())
    {
      throw std::overflow_error ("offset + index > BitT capacity");
    }
  if (offset > bit_capacity<RetT>())
  {
      throw std::overflow_error ("offset > RetT capacity");
  }
  BitT offsetmask = max_value<BitT> () >> (bit_capacity<BitT> () - offset);
  return (RetT) (bits >> index) & offsetmask;
}

__BITF_UINT_TT (BitT, RetT)
// get vector of n atomic values from bitdata
std::vector<RetT>
get_vector (BitT bits, int index, size_t offset, size_t n)
{
  index &= max_index<BitT> ();

  if (offset * n + index > bit_capacity<BitT> ())
    {
      throw std::overflow_error ("offset + index > BitT capacity");
    }
  if (offset > bit_capacity<RetT>())
  {
      throw std::overflow_error ("offset > RetT capacity");
  }

  size_t maxn = (bit_capacity<BitT> () - index) / offset;
  if (n > maxn)
  {
    throw std::overflow_error ("vector size > BitT capacity");
  }
  std::vector<RetT> res{};
  
  BitT offsetmask = max_value<BitT> () >> (bit_capacity<BitT> () - offset);

  for (size_t i = 0; i < n; i++)
    {
      size_t rshift = (index + (offset * i));
      BitT shifted = bits >> rshift;
      BitT val = (shifted & offsetmask);
      res.push_back ((RetT) val);
    }
  return res;
}

__BITF_UINT_TT (BitT, ScalT)
// insert atomic value to bitfield
BitT
insert_scalar (ScalT value, int index, size_t offset, BitT bits = 0)
{
  index &= max_index<BitT> ();
  if (offset + index > bit_capacity<BitT> ())
    {
      throw std::overflow_error ("offset + index > BitT capacity");
    }
  if (bit_size<ScalT>(value) > offset)
  {
    throw std::overflow_error ("value bit width > offset");
  }
  BitT offsetmask = max_value<BitT> () >> (bit_capacity<BitT> () - (offset + index));
  BitT indexmask = offsetmask >> offset;
  BitT mask = ~(offsetmask ^ indexmask);
  bits &= mask;
  bits |= (value << index);
  return bits;
}

__BITF_UINT_TT (BitT, ScalT)
// insert vector of atomic values to bitfield
BitT
insert_vector (std::vector<ScalT> values, int index, size_t offset, BitT bits = 0)
{
  size_t n = values.size ();
  if (!n)
    {
      return bits;
    }

  index &= max_index<BitT> ();
  if (offset*n + index > bit_capacity<BitT> ())
    {
      throw std::overflow_error ("offset + index > BitT capacity");
    }

  size_t maxn = (bit_capacity<BitT> () - index) / offset;
  if (n > maxn)
    {
      throw std::overflow_error ("vector size > BitT capacity");
    }

  ScalT maxvalue = values[0];
  size_t i = 0;
  while (i < n)
    {
      if (values[i] > maxvalue)
        {
          maxvalue = values[i];
        }
      i++;
    }
  if (bit_size<ScalT>(maxvalue) > offset)
    {
      throw std::overflow_error ("vector value > offset");
    }

  BitT offsetmask = max_value<BitT> () >> (bit_capacity<BitT> () - (offset * n + index));
  BitT indexmask = offsetmask >> (offset * n);
  BitT mask = ~(offsetmask ^ indexmask);
  bits &= mask;
  i = 0;
  while (i < n)
    {
      bits |= (values[i] << (offset * i + index));
      i++;
    }
  return bits;
}

__BITF_UINT_T (T) class data
{
protected:
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

__BITF_UINT_T (T) class constructor : public virtual data<T>
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

__BITF_UINT_T (T) class accessor : public virtual data<T>
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

__BITF_UINT_T (T) class mutator : public virtual data<T>
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