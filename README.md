# BITF

flexible generic bitfields
___

## SOLID BITFIELDS
namespace **bitf::solid** contains generic functions for granular bitwise operations on unsigned integers and OOP wrappers on it

### pure functions example
```c++
uint8_t a{0b101011};
cout << bitf::solid::to_string<uint8_t>(a) << endl;
// expected output: "00101011"

a = bitf::solid::set_scalar<uint8_t>(
    1,  // insert value 1
    a   // to bitfield target x 
    1,  // offset 1
    7,  // indent 7
    ); 

// expected value of a: 0b10101011

//note that scalar values type can be different from bitfield type
int scalar = bitf::solid::get_scalar<int>(
    a,  // getting few bits as single value from bitfield source x
    3,  // offset 3
    3   // indent 3
    ); 
// expected scalar value: 0b101                    

// generic update, 
std::vector<int> vec { 7, 0, 6, 1, 5, 2, 4, 3 };
size_t bits = solid::set_bulk(
    vec_3.begin(),  // starting iterator
    vec_3.end(),    // ending iterator
    0UL,            // target bits
    3               // offset
    );             
// expected value of bits: 0b011100010101001110000111

//same generic function with raw arrays
int raw_arr[8] { 7, 0, 6, 1, 5, 2, 4, 3 };
size_t val_4 = solid::set_bulk(raw_arr, raw_arr+8, 0UL, 3);
// expected value of bits: 0b011100010101001110000111
int bucket[4];
solid::get_bulk(
    bucket,         // starting iterator
    bucket+4,       // ending iterator
    val_4,          // target bits
    3,              // offset
    2               // indent
    );
// expected value of bucket: { 2, 3, 4, 1 }
```
    
### example of class design and usage
```c++
class BitField :
public virtual bitf::solid::data<size_t>,
public virtual bitf::solid::constructor<size_t>, 
public virtual bitf::solid::accessor<size_t>,
public virtual bitf::solid::mutator<size_t>  // exclude mutator to get immutable type
{
    public:
    using bitf::solid::constructor<size_t>::constructor;
};

BitField x {};          // initialized to 0
x.set_vector(
    {1,0,2,0,3},        // insert data
    2                   // offset
    1,                  // indent
    );                  
// expected value of x.bits(): 0b11001000010
    
```
## CHUNKED BITFIELDS
namespace **bitf::chunked** extends granular functionality on generic chunked storage types with both fixed and dynamic size  
*work in progress* 