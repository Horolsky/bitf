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

__GENERIC_UNSIGNED_TYPE(T)
std::string
to_str (const T *const chunks, size_t n)
{
  const T CHUNK_SIZE = solid::bit_size<T> ();
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
__GENERIC_UNSIGNED_TYPES(ChunkT, ScalarT) 
ScalarT
get_scalar (ChunkT const *chunks, size_t n, int index, const size_t offset)
{
  const size_t CHUNK_SIZE = solid::bit_size<ChunkT> ();
  const size_t MAX_OFFSET = solid::bit_size<ScalarT> ();
  const size_t MAX_SCALAR = solid::max_value<ScalarT> ();
  const size_t BITS = CHUNK_SIZE * n;

  index &= BITS - 1;
  if (offset > MAX_OFFSET)
    {
      throw std::overflow_error ("offset > max offset");
    }
  if (offset + index > BITS)
    {
      throw std::overflow_error ("offset + index > chunks bitsize");
    }
  ScalarT res{ 0 };
  size_t i = index % CHUNK_SIZE; // first chunk bit index
  ChunkT const *chunk = chunks + (n - 1 - i / CHUNK_SIZE);
  ChunkT const *end = chunk - (offset + i - 1) / CHUNK_SIZE;

  // first chunk bits
  res = ((ScalarT)(*chunk)) >> i;

  // overflow chunks bits
  if (chunk != end)
    {
      i = CHUNK_SIZE - i;    // res bit index
      while (--chunk >= end) // sic, reverse indexation
        {
          res |= ((ScalarT)(*chunk)) << i;
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
    return solid::bit_size<T> () * N;
  }
};

/**
 * dynamic bitdata storage struct
 * T: chunk type, unsigned integer only
 */
__GENERIC_UNSIGNED_TYPE(T)
struct _dynamic
{
  std::vector<T> _data{};
  size_t
  chunks () const
  {
    return _data.size ();
  }
  size_t
  bits () const
  {
    return solid::bit_size<T> () * _data.size ();
  }
};

} // namespace bitf::chunked
} // namespace bitf