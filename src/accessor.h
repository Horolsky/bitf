#pragma once
#include "data.h"
namespace bitf
{
    template <class T,
              std::enable_if_t<std::is_unsigned<T>::value, bool> = true>
    class accessor : public virtual data<T>
    {
    public:
        //get atomic value from bitdata
        T get(int index, T offset) const
        {
            index &= data<T>::MAXINDEX;
            offset = (--offset & data<T>::MAXINDEX) + 1;
            T offsetmask = data<T>::MAX >> (data<T>::BITSIZE - offset);
            return (data<T>::_bits >> index) & offsetmask;
        };
        //get vector of n atomic values from bitdata
        std::vector<T> get(int index, T offset, T n) const
        {
            index &= data<T>::MAXINDEX;

            offset = (--offset & data<T>::MAXINDEX) + 1;
            T maxn = (data<T>::BITSIZE - index) / offset;
            n = n > maxn ? maxn : n;

            std::vector<T> res{};
            T offsetmask = data<T>::MAX >> (data<T>::BITSIZE - offset);

            for (size_t i = 0; i < n; i++)
            {
                T rshift = (index + (offset * i));
                T shifted = data<T>::_bits >> rshift;
                T val = (shifted & offsetmask);
                res.push_back(val);
            }

            return res;
        };
    };
} 
