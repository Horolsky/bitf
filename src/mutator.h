#pragma once
#include "data.h"
//TODO: possibly inherit mutator from accessor
namespace bitf
{
    template <class T,
              std::enable_if_t<std::is_unsigned<T>::value, bool> = true>
    class mutator : public virtual data<T>
    {
    public:
    
        //set bits value
        void set(T value)
        {
            data<T>::_bits = value;
        };

        //insert atomic value to bitfield
        void insert(T value, int index, T offset)
        {
            index &= data<T>::MAXINDEX;
            offset = (--offset & data<T>::MAXINDEX) + 1;
            if (data<T>::nofbits(value) > offset)
                throw;
            T offsetmask = data<T>::MAX >> (data<T>::BITSIZE - offset);
            T mask = ~(offsetmask << index);
            data<T>::_bits &= mask;
            data<T>::_bits |= (value << index);
        };

        //insert vector of atomic values to bitfield
        void insert(std::vector<T> values, int index, T offset)
        {
            T n = values.size();
            if (!n) return;

            index &= data<T>::MAXINDEX;
            offset = (--offset & data<T>::MAXINDEX) + 1;
            T maxn = (data<T>::BITSIZE - index) / offset;
            if (n > maxn)
                throw;

            T maxvalue = values[0];
            size_t i = 0;
            while (i < n)
            {
                if (values[i] > maxvalue)
                    maxvalue = values[i];
                i++;
            }
            if (data<T>::nofbits(maxvalue) > offset)
                throw;
            T offsetmask = data<T>::MAX >> ((data<T>::MAXINDEX - offset - 1) * n);
            T mask = ~(offsetmask << index);
            data<T>::_bits &= mask;
            i = 0;
            while (i < n)
            {
                data<T>::_bits |= (values[i] << (offset * i + index));
                i++;
            }
        };
    };
} 
