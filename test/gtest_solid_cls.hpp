#pragma once

#include "bitf_solid.hpp"
#include "gtest/gtest.h"

#include <vector>

using namespace bitf;

// test class
class BitF : public virtual solid::data<size_t>,
             public virtual solid::constructor<size_t>,
             public virtual solid::accessor<size_t>,
             public virtual solid::mutator<size_t>
{
public:
  using solid::constructor<size_t>::constructor;
};

#pragma region TEST_DATA

TEST (solid_data, init_def)
{
  BitF x{};
  EXPECT_EQ (x.bits (), 0b0);
}

TEST (solid_data, init_zero)
{
  BitF x{ 0 };
  EXPECT_EQ (x.bits (), 0b0);
}

TEST (solid_data, init_max)
{
  BitF x{ solid::max_value<size_t> () };
  EXPECT_EQ (x.bits (), solid::max_value<size_t> ());
}

TEST (solid_data, copy_constructor)
{
  BitF a{ 474434687 };
  BitF b = a;
  EXPECT_EQ (a.bits (), b.bits ());
}

TEST (solid_data, assignment)
{
  BitF a{ 474434687 };
  BitF b{ 0 };
  b = a;
  EXPECT_EQ (a.bits (), b.bits ());
}
TEST (solid_data, comparison)
{
  BitF a{ 474434687 };
  BitF b{ 0 };
  EXPECT_NE (a, b);
  b = a;
  EXPECT_EQ (a, b);
}

#pragma endregion

#pragma region TEST_CONSTRUCTOR

TEST (solid_constructor, update_bits)
{
  size_t bits = 0b101010;

  BitF x{ 1, 0, bits };

  EXPECT_EQ (x.bits (), 0b101011);
}

TEST (solid_constructor, set_vector)
{
  std::vector<size_t> vec{ 1, 0, 2, 0, 3 };
  BitF a{ vec, 0, 2 };

  bool equal = true;
  auto ret_vec = a.get_vector (0, 2, vec.size ());

  for (size_t i = 0; i < vec.size (); i++)
    if (vec[i] != ret_vec[i])
      equal = false;

  EXPECT_EQ (equal, true);
}

TEST (solid_constructor, vector_update)
{
  size_t bits = 0b10000000;
  std::vector<size_t> vec{ 1, 2, 3 };
  BitF a{ vec, 0, 2, bits };
  EXPECT_EQ (a.bits (), 0b10111001);
}

#pragma endregion

#pragma region TEST_ACCESSOR

TEST (solid_accessor, get_scalar)
{
  BitF x1{ 0b10101 };
  size_t a1 = x1.get_scalar (2, 1);
  EXPECT_EQ (a1, 0b1);

  BitF x2{ 0b10101 };
  size_t a2 = x2.get_scalar (2, 3);
  EXPECT_EQ (a2, 0b101);

  BitF x3{ 0b10101 };
  size_t a3 = x3[0];
  EXPECT_EQ (a3, 0b1);
}

TEST (solid_accessor, get_vector)
{
  BitF x{ 0b1100100001 };
  std::vector<size_t> vec{ 1, 0, 2, 0, 3 };
  bool equal = true;
  auto ret_vec = x.get_vector (0, 2, vec.size ());

  for (size_t i = 0; i < vec.size (); i++)
    if (vec[i] != ret_vec[i])
      equal = false;

  EXPECT_EQ (equal, true);
}

#pragma endregion

#pragma region TEST_MUTATOR

TEST (solid_mutator, set_zero)
{
  BitF x{ 243387867 };
  x.set (0);
  EXPECT_EQ (x.bits (), 0b0);
}

TEST (solid_mutator, insert_1bit)
{
  BitF x{ 0b10001 };
  x.insert_scalar (1, 2, 1);
  EXPECT_EQ (x.bits (), 0b10101);
}

TEST (solid_mutator, insert_3bits)
{
  BitF x{ 0b10000001 };
  x.insert_scalar (0b111, 3, 3);
  EXPECT_EQ (x.bits (), 0b10111001);
}

TEST (solid_mutator, set_vector)
{
  BitF x{ 0 };
  x.insert_vector ({ 1, 0, 2, 0, 3 }, 1, 2);
  auto bv = x.bits ();
  EXPECT_EQ (bv, 0b11001000010);

  BitF y{ 0b10000000 };
  y.insert_vector ({ 1, 2, 3 }, 0, 2);
  EXPECT_EQ (y.bits (), 0b10111001);
}

#pragma endregion
