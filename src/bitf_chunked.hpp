#pragma once

#include "bitf_solid.hpp"

namespace bitf
{
/**
 * bitfields with generic chunked storage
 * the indexation of chunks is reverted
 * to keep it in line with bit indexation
 * i. e. rightmost is most significant
 */
namespace chunked
{

template <class T>
std::string
to_str (const T *const chunks, size_t n)
{
  _BITF_ASSERT_UNSIGNED (T);
  const T CHUNK_SIZE = solid::bit_capacity<T> ();
  std::string res (CHUNK_SIZE * n, '0');

  for (size_t i = 0; i < n; i++)
    {
      for (size_t j = 0; j < CHUNK_SIZE; j++)

        {
          if ((chunks[i] >> j) & 1)
            {
              res[CHUNK_SIZE * (i + 1) - j - 1] = '1';
            }
        }
    }
  return res;
}

/**
 * max offset = 64
 * TODO: template with params for chunk and return val
 */
template <class T, class BitT>
T
get_scalar (BitT const *chunks, size_t n, int indent, const size_t offset)
{
  _BITF_ASSERT_UNSIGNED (BitT);
  _BITF_ASSERT_INTEGRAL (T);

  const size_t CHUNK_SIZE = solid::bit_capacity<BitT> ();
  const size_t MAX_OFFSET = solid::bit_capacity<T> ();
  const size_t MAX_SCALAR = solid::max_value<T> ();
  const size_t BITS = CHUNK_SIZE * n;

  indent &= BITS - 1;
  if (offset > MAX_OFFSET)
    {
      throw std::overflow_error ("offset > max offset");
    }
  if (offset + indent > BITS)
    {
      throw std::overflow_error ("offset + indent > chunks bitsize");
    }
  T res{ 0 };
  size_t i = indent % CHUNK_SIZE; // first chunk bit indent
  BitT const *chunk = chunks + (n - 1 - i / CHUNK_SIZE);
  BitT const *end = chunk - (offset + i - 1) / CHUNK_SIZE;

  // first chunk bits
  res = ((T)(*chunk)) >> i;

  // overflow chunks bits
  if (chunk != end)
    {
      i = CHUNK_SIZE - i;    // res bit indent
      while (--chunk >= end) // sic, reverse indexation
        {
          res |= ((T)(*chunk)) << i;
          i += CHUNK_SIZE;
        }
    }
  res &= MAX_SCALAR >> (MAX_OFFSET - offset); // offset mask
  return res;
}

/**
 * fixed size bitdata storage struct
 * T: chunk type, unsigned integer only
 * N: chunks number, default: 1
 */
template <typename T, size_t N = 1,
          std::enable_if_t<std::is_unsigned<T>::value, bool> = true>
struct _static
{
  static_assert (N > 0, "bitf::chunked::_static cannot be of zero size");
  const T data[N]{ 0 };

  size_t
  chunks () const
  {
    return N;
  }
  size_t
  bits () const
  {
    return solid::bit_capacity<T> () * N;
  }
};

/**
 * dynamic bitdata storage struct
 * T: chunk type, unsigned integer only
 */
template <class T> struct _dynamic
{
  _BITF_ASSERT_UNSIGNED (T);
  std::vector<T> _data{};
  size_t
  chunks () const
  {
    return _data.size ();
  }
  size_t
  bits () const
  {
    return solid::bit_capacity<T> () * _data.size ();
  }
};

} // namespace bitf::chunked
} // namespace bitf