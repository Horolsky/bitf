#### bitf

flexible generic bitfields
___

**bitf::data**:         basic class for data storage  
**bitf::constructor**:  atomic bitfield constructors  
**bitf::accessor**:     atomic bitfield getters  
**bitf::mutator**:      atomic bitfield setters for mutable classes with same functionality as constructors  

## example of subclass design
```c++
class BitField :
public virtual data<size_t>,
public virtual constructor<size_t>, 
public virtual accessor<size_t>,
public virtual mutator<size_t> //exclude mutator to get immutable type
{
    public:
    using constructor<size_t>::constructor;
};
```

## example of usage

```c++
    BitField x {};   // initialized to 0
    x.insert(
        {1,0,2,0,3},        // atomic bitwise data
        1,                  // starting index
        2                   // offset
        );                  // x value now equal to 0b11001000010
    
    cout << x.str();        // expect to print '11001000010'
```
