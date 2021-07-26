#pragma once
#include <vector>
#include <string>
#include <type_traits>
#include <stdexcept>

namespace bitf
{
    /**
     * static template class with basic constants and functions
     * T: primitive unsigned integer type
     */ 
    template <typename T,
              std::enable_if_t<std::is_unsigned<T>::value, bool> = true>
    class func
    {
    private:
        func() {}
        ~func() {}
        func(const func &);
        func &operator=(const func &);

    public:
        //bitfield storage type
        typedef T type;
        //max value of bitfield storage type
        static const T MAX = ~0;
        //number of stored bits
        static const T BITSIZE = sizeof(T) << 0b11;
        //max index of bitfield (BITSIZE - 1)
        static const T MAXINDEX = BITSIZE - 1;
        //number of bits needed to store value
        static size_t nofbits(T value)
        {
            size_t n = 1;
            while (value >>= 1)
                n++;
            return n;
        };
        //stringify binary representation of bitfield
        static std::string str(T bits)
        {
            std::string res(BITSIZE, '0');
            for (size_t i = 0; i < BITSIZE; i++)
            {
                if ((bits >> i) & 1)
                    res[BITSIZE - i - 1] = '1';
            }
            return res;
        };
        //get atomic value from bitdata
        static T get(T bits, int index, size_t offset)
        {
            index &= MAXINDEX;
            if (offset + index > BITSIZE) throw std::overflow_error("offset + index > BITSIZE");
            T offsetmask = MAX >> (BITSIZE - offset);
            return (bits >> index) & offsetmask;
        };
        //get vector of n atomic values from bitdata
        static std::vector<T> get(T bits, int index, size_t offset, size_t n)
        {
            index &= MAXINDEX;

            if (offset + index > BITSIZE) throw std::overflow_error("offset + index > BITSIZE");
            size_t maxn = (BITSIZE - index) / offset;
            n = n > maxn ? maxn : n;

            std::vector<T> res{};
            T offsetmask = MAX >> (BITSIZE - offset);

            for (size_t i = 0; i < n; i++)
            {
                size_t rshift = (index + (offset * i));
                T shifted = bits >> rshift;
                T val = (shifted & offsetmask);
                res.push_back(val);
            }

            return res;
        };
        //insert atomic value to bitfield
        static T insert(T value, int index, size_t offset, T bits = 0)
        {
            index &= MAXINDEX;
            if (offset + index > BITSIZE) throw std::overflow_error("offset + index > BITSIZE");
            T offsetmask = MAX >> (BITSIZE - (offset + index));
            T indexmask = offsetmask >> index;
            T mask = ~(offsetmask ^ indexmask);
            bits &= mask;
            bits |= (value << index);
            return bits;
        };
        //insert vector of atomic values to bitfield
        static T insert(std::vector<T> values, int index, size_t offset, T bits = 0)
        {
            T n = values.size();
            if (!n)
                return bits;

            index &= MAXINDEX;
            if (offset + index > BITSIZE) throw std::overflow_error("offset + index > BITSIZE");

            T maxn = (BITSIZE - index) / offset;
            if (n > maxn) throw std::overflow_error("vector size > bitfield size");

            T maxvalue = values[0];
            size_t i = 0;
            while (i < n)
            {
                if (values[i] > maxvalue)
                    maxvalue = values[i];
                i++;
            }
            if (nofbits(maxvalue) > offset) throw std::overflow_error("vector value > offset");

            T offsetmask = MAX >> (BITSIZE - (offset * n + index));
            T indexmask = offsetmask >> index;
            T mask = ~(offsetmask ^ indexmask);
            bits &= mask;
            i = 0;
            while (i < n)
            {
                bits |= (values[i] << (offset * i + index));
                i++;
            }
            return bits;
        };
    };

    template <typename T>
    class data
    {
    protected:
        T _bits;
    public:
        //max value of bitfield storage type
        static T maxvalue() { return func<T>::MAX; }

        //number of stored bits
        static T bitsize() { return func<T>::BITSIZE; };

        //initialization with zero
        data() : _bits(0){};

        //initialization with given value
        data(T val) : _bits(val){};
        //copy constructor
        data(data &bd) : _bits(bd._bits){};

        data<T> &operator=(const data<T> &other)
        {
            _bits = other._bits;
            return *this;
        }

        bool operator==(const data<T> &other) const { return _bits == other._bits; }
        bool operator!=(const data<T> &other) const { return _bits != other._bits; }

        //full bitfield value
        T bits() const { return _bits; };

        //stringify binary representation of bitfield
        std::string str() const { return func<T>::str(_bits); };
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
            data<T>::_bits = func<T>::insert(value, index, func<T>::nofbits(value), bits);
        };
        /**
         * create bitfield with vector of atomic values
         * other bits are set to 0
         */
        constructor(std::vector<T> values, int index, size_t offset, T bits = 0)
        {
            data<T>::_bits = func<T>::insert(values, index, offset, bits);
        };
    };

    template <class T>
    class accessor : public virtual data<T>
    {
    public:
        //get atomic value from bitdata
        T get(int index, size_t offset) const { return func<T>::get(data<T>::_bits, index, offset); };
        //get vector of n atomic values from bitdata
        std::vector<T> get(int index, size_t offset, T n) const { return func<T>::get(data<T>::_bits, index, offset, n); };
    
    
    };

    template <class T>
    class mutator : public virtual data<T>
    {
    public:
        //set bits value
        void set(T value) { data<T>::_bits = value; };

        //insert atomic value to bitfield
        void insert(T value, int index, size_t offset) { data<T>::_bits = func<T>::insert(value, index, offset, data<T>::_bits); };

        //insert vector of atomic values to bitfield
        void insert(std::vector<T> values, int index, size_t offset) { data<T>::_bits = func<T>::insert(values, index, offset, data<T>::_bits); };
    };
}
