#pragma once

#include "bitf_chunked.hpp"
#include "gtest/gtest.h"

using namespace bitf;

TEST (chunked_func, get_scalar)
{
    uint8_t chunks[3] { 0b10101010, 0b11001100, 0b11110000 };
    EXPECT_EQ("101010101100110011110000", chunked::to_str(chunks, 3));
    uint16_t a;
    a = chunked::get_scalar<uint8_t, uint16_t>(chunks, 3, 0, 5);
    EXPECT_EQ(0b0000000000010000, a);
    a = chunked::get_scalar<uint8_t, uint16_t>(chunks, 3, 1, 5);
    EXPECT_EQ(0b0000000000011000, a);
    a = chunked::get_scalar<uint8_t, uint16_t>(chunks, 3, 2, 5);
    EXPECT_EQ(0b0000000000011100, a);
    a = chunked::get_scalar<uint8_t, uint16_t>(chunks, 3, 3, 5);
    EXPECT_EQ(0b0000000000011110, a);
    a = chunked::get_scalar<uint8_t, uint16_t>(chunks, 3, 0, 9);
    EXPECT_EQ(0b0000000011110000, a);
    a = chunked::get_scalar<uint8_t, uint16_t>(chunks, 3, 0, 12);
    EXPECT_EQ(0b0000110011110000, a);
    a = chunked::get_scalar<uint8_t, uint16_t>(chunks, 3, 1, 12);
    EXPECT_EQ(0b0000011001111000, a);
    a = chunked::get_scalar<uint8_t, uint16_t>(chunks, 3, 4, 12);
    EXPECT_EQ(0b0000110011001111, a);
}
