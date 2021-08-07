# BITF

flexible generic bitfields
__

## SCALAR BITFIELDS
namespace **bitf::scalar** contains pure generic functions for granular bitwise operations on unsigned integers and OOP wrappers on it

## pure functions example
```c++
uint8_t x{0b101011};
cout << bitf::scalar::to_str<uint8_t>(x) << endl;
// expected output: "00101011"

x = bitf::scalar::insert<uint8_t>(
    1,  // inserting bit '1'
    7,  // to position with index 7
    1,  // with bit offset 1
    x   // to bitfield target x (default to 0)
    ); 

cout << bitf::scalar::to_str<uint8_t>(x) << endl;
// expected output: "10101011"

uint8_t bits = bitf::scalar::get<uint8_t>(
    x,  // getting few bits as single value from bitfield source x
    3,  // from index 3
    3   // offset 3
    );  // i. e. 0b101 or 5

cout << bitf::scalar::to_str<uint8_t>(bits) << endl;
// expected output: "00000101"                    
```
    
## example of class design and usage
```c++
class BitField :
public virtual bitf::scalar::data<size_t>,
public virtual bitf::scalar::constructor<size_t>, 
public virtual bitf::scalar::accessor<size_t>,
public virtual bitf::scalar::mutator<size_t>  // exclude mutator to get immutable type
{
    public:
    using bitf::scalar::constructor<size_t>::constructor;
};

BitField x {};          // initialized to 0
x.insert(
    {1,0,2,0,3},        // atomic bitwise data
    1,                  // starting index
    2                   // offset
    );                  // x value now equal to 0b11001000010
    
cout << x.to_str();     // expect to print '11001000010'
```
