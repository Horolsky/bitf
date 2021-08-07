#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace bitf
{
/**
 * bitfields with generic scalar storage
 */
namespace scalar
{
#define __GENERIC_UNSIGNED_T                                                  \
  template <typename T,                                                       \
            std::enable_if_t<std::is_unsigned<T>::value, bool> = true>

// max value of bitfield storage type
__GENERIC_UNSIGNED_T
inline T
max_value ()
{
  return ~0;
}

// number of stored bits
__GENERIC_UNSIGNED_T
inline size_t
bit_size ()
{
  return sizeof (T) << 0b11;
}

// max bitfield index with offset = 1
__GENERIC_UNSIGNED_T
inline size_t
max_index ()
{
  return (sizeof (T) << 0b11) - 1;
}

// number of bits needed to store value
__GENERIC_UNSIGNED_T
size_t
bit_width (T value)
{
  size_t n = 1;
  while (value >>= 1)
    {
      n++;
    }
  return n;
}

#define max_value(T) max_value<T> ()
#define bit_size(T) bit_size<T> ()
#define max_index(T) max_index<T> ()

// stringify binary representation of bitfield
__GENERIC_UNSIGNED_T
std::string
to_str (T bits)
{
  size_t size = bit_size (T);
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
__GENERIC_UNSIGNED_T
T
get (T bits, int index, size_t offset)
{
  index &= max_index (T);
  if (offset + index > bit_size (T))
    {
      throw std::overflow_error ("offset + index > BITSIZE");
    }
  T offsetmask = max_value (T) >> (bit_size (T) - offset);
  return (bits >> index) & offsetmask;
}

// get vector of n atomic values from bitdata
__GENERIC_UNSIGNED_T
std::vector<T>
get (T bits, int index, size_t offset, size_t n)
{
  index &= max_index (T);

  if (offset + index > bit_size (T))
    {
      throw std::overflow_error ("offset + index > BITSIZE");
    }
  size_t maxn = (bit_size (T) - index) / offset;
  n = n > maxn ? maxn : n;

  std::vector<T> res{};
  T offsetmask = max_value (T) >> (bit_size (T) - offset);

  for (size_t i = 0; i < n; i++)
    {
      size_t rshift = (index + (offset * i));
      T shifted = bits >> rshift;
      T val = (shifted & offsetmask);
      res.push_back (val);
    }

  return res;
}
// insert atomic value to bitfield
__GENERIC_UNSIGNED_T
T
insert (T value, int index, size_t offset, T bits = 0)
{
  index &= max_index (T);
  if (offset + index > bit_size (T))
    {
      throw std::overflow_error ("offset + index > BITSIZE");
    }
  T offsetmask = max_value (T) >> (bit_size (T) - (offset + index));
  T indexmask = offsetmask >> offset;
  T mask = ~(offsetmask ^ indexmask);
  bits &= mask;
  bits |= (value << index);
  return bits;
}
// insert vector of atomic values to bitfield
__GENERIC_UNSIGNED_T
T
insert (std::vector<T> values, int index, size_t offset, T bits = 0)
{
  T n = values.size ();
  if (!n)
    {
      return bits;
    }

  index &= max_index (T);
  if (offset + index > bit_size (T))
    {
      throw std::overflow_error ("offset + index > BITSIZE");
    }

  T maxn = (bit_size (T) - index) / offset;
  if (n > maxn)
    {
      throw std::overflow_error ("vector size > bitfield size");
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
  if (bit_width (maxvalue) > offset)
    {
      throw std::overflow_error ("vector value > offset");
    }

  T offsetmask = max_value (T) >> (bit_size (T) - (offset * n + index));
  T indexmask = offsetmask >> (offset * n);
  T mask = ~(offsetmask ^ indexmask);
  bits &= mask;
  i = 0;
  while (i < n)
    {
      bits |= (values[i] << (offset * i + index));
      i++;
    }
  return bits;
}

} // namespace bitf::scalar

} // namespace bitf