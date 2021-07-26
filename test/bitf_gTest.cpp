#include "bitf.hpp"

#include "gtest/gtest.h"

#include <vector>
#include <algorithm>
using namespace bitf;

//test class
class BitF : 
public virtual data<size_t>,
public virtual constructor<size_t>,
public virtual accessor<size_t>,
public virtual mutator<size_t>
{
    public:
    using constructor<size_t>::constructor;
};

#pragma region TEST_FUNC_CONST

TEST(func, const_bitsize)
{
    auto s_uc =  func<unsigned char>::BITSIZE;
	EXPECT_EQ(s_uc, sizeof(unsigned char) << 0b11);

    auto s_ud =  func<unsigned int>::BITSIZE;
	EXPECT_EQ(s_ud, sizeof(unsigned int) << 0b11);

    auto s_ul =  func<size_t>::BITSIZE;
	EXPECT_EQ(s_ul, sizeof(size_t) << 0b11);
}

TEST(func, const_max)
{
    auto max_ul =  func<size_t>::MAX;
    EXPECT_EQ(max_ul, ~0UL);

    auto max_ud =  func<unsigned int>::MAX;
    EXPECT_EQ(max_ud, ~0U);

    auto max_uc =  func<unsigned char>::MAX;
    EXPECT_EQ(max_uc, 255);
}

#pragma endregion

#pragma region TEEST_FUNC_UTILS

TEST(func, util_nofbits)
{
    EXPECT_EQ(func<size_t>::nofbits(0b10101), 5);
    EXPECT_EQ(func<size_t>::nofbits(0b1000100010001000), 16);
}
TEST(func, util_str)
{
    EXPECT_EQ(func<unsigned char>::str(0b1010), "00001010");
    EXPECT_EQ(func<unsigned char>::str(255), "11111111");
}

#pragma endregion

#pragma region TEEST_FUNC_GETTERS

TEST(func, get_value)
{
    EXPECT_EQ(func<size_t>::get(0b10000101, 2, 1), 1);
    EXPECT_EQ(func<size_t>::get(0b10001000, 3, 1), 1);

    auto index = func<size_t>::MAXINDEX;
    auto left = 1UL << index;
    EXPECT_EQ(func<size_t>::get(left, index, 1), 1);
    EXPECT_EQ(func<size_t>::get(left, 0, index), 0);

    //offset test
    EXPECT_EQ(func<size_t>::get(0b10011100001, 0, 5), 0b1);
    EXPECT_EQ(func<size_t>::get(0b10011100001, 1, 4), 0b0);
    EXPECT_EQ(func<size_t>::get(0b10011100001, 5, 3), 0b111);
    EXPECT_EQ(func<size_t>::get(0b10011100001, 5, 4), 0b111);
    EXPECT_EQ(func<size_t>::get(0b10011100001, 5, 5), 0b111);
    EXPECT_EQ(func<size_t>::get(0b10011100001, 5, 6), 0b100111);
}
TEST(func, get_vector)
{
    auto check_vector = [](size_t bits, int index, size_t offset, std::vector<size_t> &b)
    {
        auto a = func<size_t>::get(bits, index, offset, b.size());
        return std::equal(a.begin(),a.end(),b.begin());
    };
    
    size_t val_1 {0b00010001};
    std::vector<size_t> vec_1 {1,0,0,0,1,0,0,0};
    EXPECT_TRUE(check_vector(val_1, 0, 1, vec_1));

    size_t val_2 {0b1110010011100100};
    std::vector<size_t> vec_2 {0,1,2,3,0,1,2,3};
    EXPECT_TRUE(check_vector(val_2, 0, 2, vec_2));

    size_t val_3 {0b011100010101001110000111};
    std::vector<size_t> vec_3 {7,0,6,1,5,2,4,3};
    EXPECT_TRUE(check_vector(val_3, 0, 3, vec_3));
}

#pragma endregion

#pragma region TEEST_FUNC_SETTERS

TEST(func, set_vector)
{
    std::vector<size_t> vec_1 {1,0,0,0,1,0,0,0};
    auto val_1 = func<size_t>::insert(vec_1, 0, 1, 0);
    EXPECT_EQ(val_1, 0b00010001);

    std::vector<size_t> vec_2 {0,1,2,3,0,1,2,3};
    auto val_2 = func<size_t>::insert(vec_2, 0, 2, 0);
    EXPECT_EQ(val_2, 0b1110010011100100);

    std::vector<size_t> vec_3 {7,0,6,1,5,2,4,3};
    auto val_3 = func<size_t>::insert(vec_3, 0, 3, 0);
    EXPECT_EQ(val_3, 0b011100010101001110000111);
}

#pragma endregion

#pragma region TEST_DATA

TEST(data, init_def)
{
    BitF x {};
    EXPECT_EQ(x.bits(), 0b0);
}

TEST(data, init_zero)
{
    BitF x {0};
    EXPECT_EQ(x.bits(), 0b0);
}

TEST(data, init_max)
{
    BitF x {BitF::maxvalue()};
    EXPECT_EQ(x.bits(), BitF::maxvalue());
}

TEST(data, copy_constructor)
{
    BitF a {474434687};
    BitF b = a;
	EXPECT_EQ(a.bits(), b.bits());
}

TEST(data, assignment)
{
    BitF a {474434687};
    BitF b {0};
    b = a;
	EXPECT_EQ(a.bits(), b.bits());
}
TEST(data, comparison)
{
    BitF a {474434687};
    BitF b {0};
	EXPECT_NE(a, b);
    b = a;
    EXPECT_EQ(a, b);
}

#pragma endregion

#pragma region TEST_CONSTRUCTOR

TEST(constructor, update_bits)
{
    size_t bits = 0b101010;
    
    BitF x {1, 0, bits};

	EXPECT_EQ(x.bits(), 0b101011);
}

TEST(constructor, set_vector)
{
    std::vector<size_t> vec {1,0,2,0,3};
    BitF a {vec, 0, 2};
    
    bool equal = true;
    auto ret_vec = a.get(0,2,vec.size());
     
    for (size_t i = 0; i < vec.size(); i++)
        if (vec[i] != ret_vec[i]) equal = false;

	EXPECT_EQ(equal, true);
}

TEST(constructor, vector_update)
{
    size_t bits = 0b10000000;
    std::vector<size_t> vec {1,2,3};
    BitF a {vec,0, 2, bits}; 
	EXPECT_EQ(a.bits(), 0b10111001);
}

#pragma endregion

#pragma region TEST_ACCESSOR

TEST(accessor, get_value)
{
    BitF x1 {0b10101};
    size_t a1 = x1.get(2, 1);
	EXPECT_EQ(a1, 0b1);

    BitF x2 {0b10101};
    size_t a2 = x2.get(2, 3);
	EXPECT_EQ(a2, 0b101);

    BitF x3 {0b10101};
    size_t a3 = x3[0];
	EXPECT_EQ(a3, 0b1);
}


TEST(accessor, get_vector)
{
    BitF x {0b1100100001};
    std::vector<size_t> vec {1,0,2,0,3};
    bool equal = true;
    auto ret_vec = x.get(0,2,vec.size());
     
    for (size_t i = 0; i < vec.size(); i++)
        if (vec[i] != ret_vec[i]) equal = false;

	EXPECT_EQ(equal, true);
}

#pragma endregion

#pragma region TEST_MUTATOR

TEST(mutator, set_zero)
{
    BitF x {243387867};
    x.set(0);
	EXPECT_EQ(x.bits(), 0b0);
}

TEST(mutator, insert_1bit)
{
    BitF x {0b10001};
    x.insert(1,2,1);
	EXPECT_EQ(x.bits(), 0b10101);
}

TEST(mutator, insert_3bits)
{
    BitF x {0b10000001};
    x.insert(0b111,3,3);
	EXPECT_EQ(x.bits(), 0b10111001);
}


TEST(mutator, set_vector)
{
    BitF x {0};
    x.insert({1,0,2,0,3},1,2);
    auto bv = x.bits();
	EXPECT_EQ(bv, 0b11001000010);
}
TEST(mutator, set_vector2)
{
    BitF x {0b10000000};
    x.insert({1,2,3}, 0,2);
	EXPECT_EQ(x.bits(), 0b10111001);
}

#pragma endregion

int main(int argc, char **argv)
{
  printf("Running %s...\n", argv[0]);

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}