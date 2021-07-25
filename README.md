# BITF

flexible generic bitfields in a single header
___
## MAIN FUNCTIONALITY
**bitf::func&lt;T&gt;**:         static generic class for granular bitwise operations on unsigned integer of type **T**
                            
## OOP WRAPPING
**bitf::data&lt;T&gt;**:         data storage class
**bitf::constructor&lt;T&gt;**:  granular constructors  
**bitf::accessor&lt;T&gt;**:     granular getters  
**bitf::mutator&lt;T&gt;**:      granular setters for mutable classes with same functionality as constructors  

## example of clear functions usage
```c++
uint8_t x{0b101011};
cout << func<uint8_t>::str(x) << endl;
// expected output: "00101011"

x = func<uint8_t>::insert(1,  // inserting bit '1'
                          7,  // to position with index 7
                          1,  // with bit offset 1
                          x); // to bitfield target x (default to 0)

cout << func<uint8_t>::str(x) << endl;
// expected output: "10101011"

uint8_t bits = func<uint8_t>::get(x,  // getting few bits as single value from bitfield source x
                                  3,  // from index 3
                                  3); // offset 3
                                      // i. e. 0b101 or 5

cout << func<uint8_t>::str(bits) << endl;
// expected output: "00000101"                    
```
    
## example of class design and usage
```c++
class BitField :
public virtual data<size_t>,
public virtual constructor<size_t>, 
public virtual accessor<size_t>,
public virtual mutator<size_t>  // exclude mutator to get immutable type
{
    public:
    using constructor<size_t>::constructor;
};

BitField x {};          // initialized to 0
x.insert(
    {1,0,2,0,3},        // atomic bitwise data
    1,                  // starting index
    2                   // offset
    );                  // x value now equal to 0b11001000010
    
cout << x.str();        // expect to print '11001000010'
```
