#pragma once

#include "bitf_solid.hpp"
#include "gtest/gtest.h"

#include <vector>

using namespace bitf;

#pragma region TEST_FUNC_CONST

TEST (solid_func, bit_capacity)
{
  auto s_uc = solid::bit_capacity<unsigned char> ();
  EXPECT_EQ (s_uc, sizeof (unsigned char) << 0b11);

  auto s_ud = solid::bit_capacity<unsigned int> ();
  EXPECT_EQ (s_ud, sizeof (unsigned int) << 0b11);

  auto s_ul = solid::bit_capacity<size_t> ();
  EXPECT_EQ (s_ul, sizeof (size_t) << 0b11);
}

TEST (solid_func, max_value)
{
  auto max_uc = solid::max_value<unsigned char> ();
  EXPECT_EQ (max_uc, 255);

  auto max_ud = solid::max_value<unsigned int> ();
  EXPECT_EQ (max_ud, ~0U);

  auto max_ul = solid::max_value<size_t> ();
  EXPECT_EQ (max_ul, ~0UL);
}

#pragma endregion

#pragma region TEST_FUNC_UTILS

TEST (solid_func, util_nofbits)
{
  EXPECT_EQ (solid::bit_size<size_t> (0b10101), 5);
  EXPECT_EQ (solid::bit_size<size_t> (0b1000100010001000), 16);
}

TEST (solid_func, util_str)
{
  EXPECT_EQ (solid::to_str<unsigned char> (0b1010), "00001010");
  EXPECT_EQ (solid::to_str<unsigned char> (255), "11111111");
}

#pragma endregion

#pragma region TEST_FUNC_GETTERS

TEST (solid_func, get_scalar)
{
  EXPECT_EQ (solid::get_scalar<size_t> (0b10000101U, 2, 1), 1);
  EXPECT_EQ (solid::get_scalar<size_t> (0b10001000U, 3, 1), 1);

  auto index = solid::max_index<size_t> ();
  auto left = 1UL << index;
  EXPECT_EQ (solid::get_scalar<size_t> (left, index, 1), 1);
  EXPECT_EQ (solid::get_scalar<size_t> (left, 0, index), 0);

  // offset test
  EXPECT_EQ (solid::get_scalar<size_t> (0b10011100001U, 0, 5), 0b1);
  EXPECT_EQ (solid::get_scalar<size_t> (0b10011100001U, 1, 4), 0b0);
  EXPECT_EQ (solid::get_scalar<size_t> (0b10011100001U, 5, 3), 0b111);
  EXPECT_EQ (solid::get_scalar<size_t> (0b10011100001U, 5, 4), 0b111);
  EXPECT_EQ (solid::get_scalar<size_t> (0b10011100001U, 5, 5), 0b111);
  EXPECT_EQ (solid::get_scalar<size_t> (0b10011100001U, 5, 6), 0b100111);
}

TEST (solid_func, collect)
{
  const size_t BITS = 0b011100010101001110000111UL;
  std::array<size_t, 8> arr1{ 7, 0, 6, 1, 5, 2, 4, 3 };
  std::array<int, 8> arr2{};
  bitf::solid::collect(
      arr2.begin (), arr2.end (), BITS, 3);

  std::vector<int> vec{};
  bitf::solid::collect(
      vec.begin (), vec.end (), BITS, 3);

  int raw_arr[8];
  bitf::solid::collect(
      raw_arr, raw_arr+8, BITS, 3);


  EXPECT_TRUE (std::equal (arr1.begin (), arr1.end (), arr2.begin ()));
  EXPECT_TRUE (std::equal (vec.begin (), vec.end (), arr2.begin ()));
  EXPECT_TRUE (std::equal (std::begin(raw_arr), std::end (raw_arr), arr2.begin ()));
}

#pragma endregion

#pragma region TEST_FUNC_SETTERS

TEST (solid_func, fill_zero)
{
  std::vector<size_t> vec_1{ 1, 0, 0, 0, 1, 0, 0, 0 };
  auto val_1 = solid::update(vec_1.begin(), vec_1.end());
  EXPECT_EQ (val_1, 0b00010001);

  std::vector<size_t> vec_2{ 0, 1, 2, 3, 0, 1, 2, 3 };
  auto val_2 = solid::update(vec_2.begin(), vec_2.end(), 0UL, 2);
  EXPECT_EQ (val_2, 0b1110010011100100);

  std::vector<size_t> vec_3{ 7, 0, 6, 1, 5, 2, 4, 3 };
  auto val_3 = solid::update(vec_3.begin(), vec_3.end(), 0UL, 3);
  EXPECT_EQ (val_3, 0b011100010101001110000111);

  int raw_arr[8] { 7, 0, 6, 1, 5, 2, 4, 3 };
  auto val_4 = solid::update(raw_arr, raw_arr+8, 0UL, 3);
  EXPECT_EQ (val_4, 0b011100010101001110000111);
}

TEST (solid_func, fill_update)
{
  const size_t BITS = bitf::solid::max_value<size_t>();
  size_t bits1  = (BITS << 8) | 0b00010001UL;
  std::vector<size_t> vec_1{ 1, 0, 0, 0, 1, 0, 0, 0 };
  auto val_1 = solid::update(vec_1.begin(), vec_1.end(), BITS);
  EXPECT_EQ (val_1, bits1);

  size_t bits2  = (BITS << 16) | 0b1110010011100100UL;
  std::vector<size_t> vec_2{ 0, 1, 2, 3, 0, 1, 2, 3 };
  auto val_2 = solid::update(vec_2.begin(), vec_2.end(), BITS, 2);
  EXPECT_EQ (val_2, bits2);

  size_t bits3  = (BITS << 24) | 0b011100010101001110000111UL;
  std::vector<size_t> vec_3{ 7, 0, 6, 1, 5, 2, 4, 3 };
  auto val_3 = solid::update(vec_3.begin(), vec_3.end(), BITS, 3);
  EXPECT_EQ (val_3, bits3);

  size_t bits4  = (BITS << 24) | 0b011100010101001110000111UL;
  int raw_arr[8] { 7, 0, 6, 1, 5, 2, 4, 3 };
  auto val_4 = solid::update(raw_arr, raw_arr+8, BITS, 3);
  EXPECT_EQ (val_4, bits4);
}

#pragma endregion
