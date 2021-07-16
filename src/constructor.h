#pragma once
#include "data.h"
namespace bitf
{
    template <class T,
              std::enable_if_t<std::is_unsigned<T>::value, bool> = true>
    class constructor : public virtual data<T>
    {
    public:
        using data<T>::data;
        /**
         * create bitfield with atomic value
         * other bits are set to 0
         */
        constructor(T value, int index)
        {
            data<T>::_bits = 0;
            index &= data<T>::MAXINDEX;

            T offset = data<T>::BITSIZE - index;
            if (data<T>::nofbits(value) > offset)
                throw;
            data<T>::_bits |= (value << index);
        };
        /**
         * create bitfield with vector of atomic values
         * other bits are set to 0
         */
        constructor(std::vector<T> values, int index, T offset)
        {
            data<T>::_bits = 0;
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
