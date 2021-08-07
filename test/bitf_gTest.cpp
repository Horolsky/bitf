#include <algorithm>
#include <vector>

#include "scalar_cls.hpp"
#include "gtest/gtest.h"
using namespace bitf;

// test class
class BitF : public virtual scalar::data<size_t>,
             public virtual scalar::constructor<size_t>,
             public virtual scalar::accessor<size_t>,
             public virtual scalar::mutator<size_t>
{
public:
  using scalar::constructor<size_t>::constructor;
};

#pragma region TEST_FUNC_CONST

TEST (func, const_bit_size)
{
  auto s_uc = scalar::bit_size(unsigned char);
  EXPECT_EQ (s_uc, sizeof (unsigned char) << 0b11);

  auto s_ud = scalar::bit_size(unsigned int);
  EXPECT_EQ (s_ud, sizeof (unsigned int) << 0b11);

  auto s_ul = scalar::bit_size(size_t);
  EXPECT_EQ (s_ul, sizeof (size_t) << 0b11);
}

TEST (func, const_max_value)
{
  auto max_uc = scalar::max_value(unsigned char);
  EXPECT_EQ (max_uc, 255);

  auto max_ud = scalar::max_value(unsigned int);
  EXPECT_EQ (max_ud, ~0U);

  auto max_ul = scalar::max_value(size_t);
  EXPECT_EQ (max_ul, ~0UL);
}

#pragma endregion

#pragma region TEST_FUNC_UTILS

TEST (func, util_nofbits)
{
  EXPECT_EQ (scalar::bit_width<size_t>(0b10101), 5);
  EXPECT_EQ (scalar::bit_width<size_t>(0b1000100010001000), 16);
}

TEST (func, util_str)
{
  EXPECT_EQ (scalar::to_str<unsigned char>(0b1010), "00001010");
  EXPECT_EQ (scalar::to_str<unsigned char>(255), "11111111");
}

#pragma endregion

#pragma region TEST_FUNC_GETTERS

TEST (func, get_value)
{
  EXPECT_EQ (scalar::get<size_t> (0b10000101, 2, 1), 1);
  EXPECT_EQ (scalar::get<size_t> (0b10001000, 3, 1), 1);

  auto index = scalar::max_index(size_t);
  auto left = 1UL << index;
  EXPECT_EQ (scalar::get<size_t> (left, index, 1), 1);
  EXPECT_EQ (scalar::get<size_t> (left, 0, index), 0);

  // offset test
  EXPECT_EQ (scalar::get<size_t> (0b10011100001, 0, 5), 0b1);
  EXPECT_EQ (scalar::get<size_t> (0b10011100001, 1, 4), 0b0);
  EXPECT_EQ (scalar::get<size_t> (0b10011100001, 5, 3), 0b111);
  EXPECT_EQ (scalar::get<size_t> (0b10011100001, 5, 4), 0b111);
  EXPECT_EQ (scalar::get<size_t> (0b10011100001, 5, 5), 0b111);
  EXPECT_EQ (scalar::get<size_t> (0b10011100001, 5, 6), 0b100111);
}
TEST (func, get_vector)
{
  auto check_vector
      = [] (size_t bits, int index, size_t offset, std::vector<size_t> &b) {
          auto a = scalar::get<size_t> (bits, index, offset, b.size ());
          return std::equal (a.begin (), a.end (), b.begin ());
        };

  size_t val_1{ 0b00010001 };
  std::vector<size_t> vec_1{ 1, 0, 0, 0, 1, 0, 0, 0 };
  EXPECT_TRUE (check_vector (val_1, 0, 1, vec_1));

  size_t val_2{ 0b1110010011100100 };
  std::vector<size_t> vec_2{ 0, 1, 2, 3, 0, 1, 2, 3 };
  EXPECT_TRUE (check_vector (val_2, 0, 2, vec_2));

  size_t val_3{ 0b011100010101001110000111 };
  std::vector<size_t> vec_3{ 7, 0, 6, 1, 5, 2, 4, 3 };
  EXPECT_TRUE (check_vector (val_3, 0, 3, vec_3));
}

#pragma endregion

#pragma region TEEST_FUNC_SETTERS

TEST (func, set_vector)
{
  std::vector<size_t> vec_1{ 1, 0, 0, 0, 1, 0, 0, 0 };
  auto val_1 = scalar::insert<size_t> (vec_1, 0, 1, 0);
  EXPECT_EQ (val_1, 0b00010001);

  std::vector<size_t> vec_2{ 0, 1, 2, 3, 0, 1, 2, 3 };
  auto val_2 = scalar::insert<size_t> (vec_2, 0, 2, 0);
  EXPECT_EQ (val_2, 0b1110010011100100);

  std::vector<size_t> vec_3{ 7, 0, 6, 1, 5, 2, 4, 3 };
  auto val_3 = scalar::insert<size_t> (vec_3, 0, 3, 0);
  EXPECT_EQ (val_3, 0b011100010101001110000111);
}

#pragma endregion

#pragma region TEST_DATA

TEST (data, init_def)
{
  BitF x{};
  EXPECT_EQ (x.bits (), 0b0);
}

TEST (data, init_zero)
{
  BitF x{ 0 };
  EXPECT_EQ (x.bits (), 0b0);
}

TEST (data, init_max)
{
  BitF x{ scalar::max_value(size_t) };
  EXPECT_EQ (x.bits (), scalar::max_value(size_t));
}

TEST (data, copy_constructor)
{
  BitF a{ 474434687 };
  BitF b = a;
  EXPECT_EQ (a.bits (), b.bits ());
}

TEST (data, assignment)
{
  BitF a{ 474434687 };
  BitF b{ 0 };
  b = a;
  EXPECT_EQ (a.bits (), b.bits ());
}
TEST (data, comparison)
{
  BitF a{ 474434687 };
  BitF b{ 0 };
  EXPECT_NE (a, b);
  b = a;
  EXPECT_EQ (a, b);
}

#pragma endregion

#pragma region TEST_CONSTRUCTOR

TEST (constructor, update_bits)
{
  size_t bits = 0b101010;

  BitF x{ 1, 0, bits };

  EXPECT_EQ (x.bits (), 0b101011);
}

TEST (constructor, set_vector)
{
  std::vector<size_t> vec{ 1, 0, 2, 0, 3 };
  BitF a{ vec, 0, 2 };

  bool equal = true;
  auto ret_vec = a.get (0, 2, vec.size ());

  for (size_t i = 0; i < vec.size (); i++)
    if (vec[i] != ret_vec[i])
      equal = false;

  EXPECT_EQ (equal, true);
}

TEST (constructor, vector_update)
{
  size_t bits = 0b10000000;
  std::vector<size_t> vec{ 1, 2, 3 };
  BitF a{ vec, 0, 2, bits };
  EXPECT_EQ (a.bits (), 0b10111001);
}

#pragma endregion

#pragma region TEST_ACCESSOR

TEST (accessor, get_value)
{
  BitF x1{ 0b10101 };
  size_t a1 = x1.get (2, 1);
  EXPECT_EQ (a1, 0b1);

  BitF x2{ 0b10101 };
  size_t a2 = x2.get (2, 3);
  EXPECT_EQ (a2, 0b101);

  BitF x3{ 0b10101 };
  size_t a3 = x3[0];
  EXPECT_EQ (a3, 0b1);
}

TEST (accessor, get_vector)
{
  BitF x{ 0b1100100001 };
  std::vector<size_t> vec{ 1, 0, 2, 0, 3 };
  bool equal = true;
  auto ret_vec = x.get (0, 2, vec.size ());

  for (size_t i = 0; i < vec.size (); i++)
    if (vec[i] != ret_vec[i])
      equal = false;

  EXPECT_EQ (equal, true);
}

#pragma endregion

#pragma region TEST_MUTATOR

TEST (mutator, set_zero)
{
  BitF x{ 243387867 };
  x.set (0);
  EXPECT_EQ (x.bits (), 0b0);
}

TEST (mutator, insert_1bit)
{
  BitF x{ 0b10001 };
  x.insert (1, 2, 1);
  EXPECT_EQ (x.bits (), 0b10101);
}

TEST (mutator, insert_3bits)
{
  BitF x{ 0b10000001 };
  x.insert (0b111, 3, 3);
  EXPECT_EQ (x.bits (), 0b10111001);
}

TEST (mutator, set_vector)
{
  BitF x{ 0 };
  x.insert ({ 1, 0, 2, 0, 3 }, 1, 2);
  auto bv = x.bits ();
  EXPECT_EQ (bv, 0b11001000010);
}
TEST (mutator, set_vector2)
{
  BitF x{ 0b10000000 };
  x.insert ({ 1, 2, 3 }, 0, 2);
  EXPECT_EQ (x.bits (), 0b10111001);
}

#pragma endregion

int
main (int argc, char **argv)
{
  printf ("Running %s...\n", argv[0]);

  testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS ();
}