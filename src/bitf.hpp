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

        data<T>& operator=(const data<T>& other)
        {
            _bits = other._bits;
            return *this;
        }

        bool operator==(const data<T>& other) const
        {
            return _bits == other._bits;
        }
        bool operator!=(const data<T>& other) const
        {
            return _bits != other._bits;
        }

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

    template <class T>
    class constructor : public virtual data<T>
    {
    public:
        using data<T>::data;
        /**
         * create bitfield with atomic value
         * other bits are set to 0
         */
        constructor(T value, int index, T bits = 0)
        {
            index &= data<T>::MAXINDEX;

            T offset = data<T>::BITSIZE - index;
            if (data<T>::nofbits(value) > offset)
                throw;
            bits |= (value << index);
            data<T>::_bits = bits;
        };
        /**
         * create bitfield with vector of atomic values
         * other bits are set to 0
         */
        constructor(std::vector<T> values, int index, T offset, T bits = 0)
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
            T offsetmask = data<T>::MAX >> (data<T>::MAXINDEX - offset*n);
            T mask = ~(offsetmask << index);
            bits &= mask;
            i = 0;
            while (i < n)
            {
                bits |= (values[i] << (offset * i + index));
                i++;
            }
            data<T>::_bits = bits;
        };
    };

        template <class T>
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

        template <class T>
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
            T offsetmask = data<T>::MAX >> (data<T>::MAXINDEX - offset*n);
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
