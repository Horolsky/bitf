#include "bitf.hpp"

#include "gtest/gtest.h"

#include <vector>
using namespace bitf;

template class data<size_t>;
template class constructor<size_t>;
template class accessor<size_t>;
template class mutator<size_t>;


//test class
class BitF : 
public virtual data<size_t>,
public virtual constructor<size_t>,
public virtual accessor<size_t>,
public virtual mutator<size_t>
{
    public:
    using constructor<size_t>::constructor;
    using accessor<size_t>::get;
    using mutator<size_t>::set;
    using mutator<size_t>::insert;

    using data<size_t>::bits;
    using data<size_t>::str;

    using data<size_t>::nofbits;
    using data<size_t>::type;
    using data<size_t>::maxvalue;
    using data<size_t>::bitsize;
};

TEST(BitF, bitsize)
{
	EXPECT_EQ(BitF::bitsize(), sizeof(size_t) << 0b11);
}

TEST(BitF, nofbits)
{
	EXPECT_EQ(BitF::nofbits(0b10101), 5);
}


TEST(BitF, init_def)
{
    BitF x {};
    EXPECT_EQ(x.bits(), 0b0);
}
TEST(BitF, init_zero)
{
    BitF x {0};
    EXPECT_EQ(x.bits(), 0b0);
}
TEST(BitF, init_max)
{
    BitF x {BitF::maxvalue()};
    EXPECT_EQ(x.bits(), BitF::maxvalue());
}

TEST(bitdata, set_zero)
{
    BitF x {243387867};
    x.set(0);
	EXPECT_EQ(x.bits(), 0b0);
}

TEST(bitdata, copy_constructor)
{
    BitF a {474434687};
    BitF b = a;
	EXPECT_EQ(a.bits(), b.bits());
}

TEST(bitdata, insert_1bit)
{
    BitF x {0b10001};
    x.insert(1,2,1);
	EXPECT_EQ(x.bits(), 0b10101);
}

TEST(bitdata, isert_3bits)
{
    BitF x {0b10000001};
    x.insert(0b111,3,3);
	EXPECT_EQ(x.bits(), 0b10111001);
}

TEST(bitdata, get_onebit)
{
    BitF x {0b10101};
    size_t a = x.get(2, 1);
	EXPECT_EQ(a, 0b1);
}

TEST(bitdata, get_3bits)
{
    BitF x {0b10101};
    size_t a = x.get(2, 3);
	EXPECT_EQ(a, 0b101);
}

TEST(bitdata, set_vector)
{
    BitF x {0};
    x.insert({1,0,2,0,3},1,2);
    auto bv = x.bits();
	EXPECT_EQ(bv, 0b11001000010);
}

TEST(bitdata, get_vector)
{
    BitF x {};
    std::vector<size_t> vec {1,2,3,1,2,3};
    x.insert(vec, 5, 2);
    bool equal = true;
    auto ret_vec = x.get(5,2,vec.size());
     
    for (size_t i = 0; i < vec.size(); i++)
        if (vec[i] != ret_vec[i]) equal = false;

	EXPECT_EQ(equal, true);
}

int main(int argc, char **argv)
{
  printf("Running main() from bitdata_gTest.cpp\n");

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
