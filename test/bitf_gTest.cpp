#include "bitf.hpp"

#include "gtest/gtest.h"

#include <vector>
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

TEST(data_static, bitsize)
{
	EXPECT_EQ(BitF::bitsize(), sizeof(size_t) << 0b11);
}

TEST(data_static, nofbits)
{
	EXPECT_EQ(func<size_t>::nofbits(0b10101), 5);
}

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


TEST(accessor, get_onebit)
{
    BitF x {0b10101};
    size_t a = x.get(2, 1);
	EXPECT_EQ(a, 0b1);
}

TEST(accessor, get_3bits)
{
    BitF x {0b10101};
    size_t a = x.get(2, 3);
	EXPECT_EQ(a, 0b101);
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

int main(int argc, char **argv)
{
  printf("Running main() from bitdata_gTest.cpp\n");

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
