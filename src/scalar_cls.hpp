#pragma once

#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "scalar_func.hpp"

namespace bitf
{
/**
 * bitfields with generic scalar storage
 */
namespace scalar
{

template <typename T> class data
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
    return scalar::to_str<T>(_bits);
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
        = scalar::insert<T>(value, index, bit_width<T>(value), bits);
  };
  /**
   * create bitfield with vector of atomic values
   * other bits are set to 0
   */
  constructor (std::vector<T> values, int index, size_t offset, T bits = 0)
  {
    data<T>::_bits = scalar::insert<T>(values, index, offset, bits);
  };
  virtual ~constructor () = default;
};

template <class T> class accessor : public virtual data<T>
{
public:
  // get atomic value from bitdata
  virtual T
  get (int index, size_t offset) const
  {
    return scalar::get<T> (data<T>::_bits, index, offset);
  };
  // get vector of n atomic values from bitdata
  virtual std::vector<T>
  get (int index, size_t offset, T n) const
  {
    return scalar::get<T> (data<T>::_bits, index, offset, n);
  };
  // get single bit value by index
  virtual T
  operator[] (int index) const
  {
    return scalar::get<T> (data<T>::_bits, index, 1);
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
  insert (T value, int index, size_t offset)
  {
    data<T>::_bits = scalar::insert<T> (value, index, offset, data<T>::_bits);
  };

  // insert vector of atomic values to bitfield
  virtual void
  insert (std::vector<T> values, int index, size_t offset)
  {
    data<T>::_bits = scalar::insert<T> (values, index, offset, data<T>::_bits);
  };
};
} // namespace bitf::scalar
} // namespace bitf
