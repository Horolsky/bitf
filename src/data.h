#pragma once
#include <vector>
#include <string>
#include <type_traits>


namespace bitf
{
    template <typename T,
              std::enable_if_t<std::is_unsigned<T>::value, bool> = true>
    class data
    {
    protected:
        T _bits;

        //max value of bitfield storage type
        static const T MAX = ~0;

        //number of stored bits
        static const T BITSIZE = sizeof(T) << 0b11;
        static const T MAXINDEX = BITSIZE - 1;

    public:

        //bitfield storage type
        typedef T type;

        //max value of bitfield storage type
        static T maxvalue() { return MAX; }
        
        //number of stored bits
        static T bitsize() { return BITSIZE; };
    
        //number of bits needed to store value
        static size_t nofbits(T value) 
        {
            size_t n = 1;
            while (value >>= 1) n++;
            return n;
        };

        //initialization with zero
        data() : _bits(0) {}; 

        //initialization with given value
        data(T val) : _bits(val) {};
        //copy constructor
        data(data& bd) : _bits(bd._bits) {};
        //move constructor
        data(data&& bd) : _bits(bd._bits) {};

        //full bitfield value
        T bits()const { return _bits; }; 

        //stringify binary representation of bitfield
        std::string str() const             
        {
            std::string res(BITSIZE, '0');
            
            for (size_t i = 0; i < BITSIZE; i++)
            {
                if ((_bits >> i) & 1)
                    res[BITSIZE-i-1] = '1';
            }

            return res;
        };
    };
}
