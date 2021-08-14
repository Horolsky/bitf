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
TEST (solid_func, get_vector)
{
  auto check_vector
      = [] (size_t bits, int index, size_t offset, std::vector<size_t> &b) {
          auto a = solid::get_vector<size_t> (bits, index, offset, b.size ());
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
TEST (solid_func, get_array)
{
  auto check_array
      = [] (size_t bits, int index, size_t offset, std::array<size_t, 8> &b) {
          auto a = solid::get_array<size_t, 8> (bits, index, offset);
          return std::equal (a.begin (), a.end (), b.begin ());
        };

  size_t val_1{ 0b00010001 };
  std::array<size_t, 8> arr_1{ 1, 0, 0, 0, 1, 0, 0, 0 };
  EXPECT_TRUE (check_array (val_1, 0, 1, arr_1));

  size_t val_2{ 0b1110010011100100 };
  std::array<size_t, 8> arr_2{ 0, 1, 2, 3, 0, 1, 2, 3 };
  EXPECT_TRUE (check_array (val_2, 0, 2, arr_2));

  size_t val_3{ 0b011100010101001110000111 };
  std::array<size_t, 8> arr_3{ 7, 0, 6, 1, 5, 2, 4, 3 };
  EXPECT_TRUE (check_array (val_3, 0, 3, arr_3));
}

// TEST (solid_func, get_cont)
// {
  // using array_UL8 = std::array<size_t, 8>;
  // auto check_array
      // = [] (size_t bits, int index, size_t offset, array_UL8 &b) 
      // {
          // auto a = solid::get_cont<size_t, array_UL8> (bits, index, offset, 8);
          // return std::equal (a.begin (), a.end (), b.begin ());
        // };
// 
  // size_t val_1{ 0b00010001 };
  // array_UL8 arr_1{ 1, 0, 0, 0, 1, 0, 0, 0 };
  // EXPECT_TRUE (check_array (val_1, 0, 1, arr_1));
// 
  // size_t val_2{ 0b1110010011100100 };
  // array_UL8 arr_2{ 0, 1, 2, 3, 0, 1, 2, 3 };
  // EXPECT_TRUE (check_array (val_2, 0, 2, arr_2));
// 
  // size_t val_3{ 0b011100010101001110000111 };
  // array_UL8 arr_3{ 7, 0, 6, 1, 5, 2, 4, 3 };
  // EXPECT_TRUE (check_array (val_3, 0, 3, arr_3));
// 
  // auto check_vec
      // = [] (size_t bits, int index, size_t offset, std::vector<size_t> &b) {
          // auto a = solid::get_cont<size_t, std::vector<size_t>> (bits, index, offset, b.size ());
          // return std::equal (a.begin (), a.end (), b.begin ());
        // };
// 
  // size_t val_4{ 0b00010001 };
  // std::vector<size_t> vec_4{ 1, 0, 0, 0, 1, 0, 0, 0 };
  // EXPECT_TRUE (check_vec (val_4, 0, 1, vec_4));
// }
#pragma endregion

#pragma region TEST_FUNC_SETTERS

TEST (solid_func, set_vector)
{
  std::vector<size_t> vec_1{ 1, 0, 0, 0, 1, 0, 0, 0 };
  auto val_1 = solid::insert_vector<size_t> (vec_1, 0, 1);
  EXPECT_EQ (val_1, 0b00010001);

  std::vector<size_t> vec_2{ 0, 1, 2, 3, 0, 1, 2, 3 };
  auto val_2 = solid::insert_vector<size_t> (vec_2, 0, 2);
  EXPECT_EQ (val_2, 0b1110010011100100);

  std::vector<size_t> vec_3{ 7, 0, 6, 1, 5, 2, 4, 3 };
  auto val_3 = solid::insert_vector<size_t> (vec_3, 0, 3);
  EXPECT_EQ (val_3, 0b011100010101001110000111);
}

TEST (solid_func, set_array)
{
  std::array<size_t, 8> vec_1{ 1, 0, 0, 0, 1, 0, 0, 0 };
  auto val_1 = solid::insert_array<size_t, 8> (vec_1, 0, 1);
  EXPECT_EQ (val_1, 0b00010001U);

  std::array<size_t, 8> vec_2{ 0, 1, 2, 3, 0, 1, 2, 3 };
  auto val_2 = solid::insert_array<size_t, 8> (vec_2, 0, 2);
  EXPECT_EQ (val_2, 0b1110010011100100U);

  std::array<size_t, 8> vec_3{ 7, 0, 6, 1, 5, 2, 4, 3 };
  auto val_3 = solid::insert_array<size_t, 8> (vec_3, 0, 3);
  EXPECT_EQ (val_3, 0b011100010101001110000111U);
}


#pragma endregion
