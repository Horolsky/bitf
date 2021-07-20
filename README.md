#### bitf

flexible generic bitfields
___

**data.h**:         basic class for data storage  
**constructor.h**:  atomic bitfield constructors  
**accessor.h**:     atomic bitfield getters  
**mutator.h**:      atomic bitfield setters for mutable classes with same functionality as constructors  

## example of subclass design
```c++
class ImmutableBitField :
public virtual data<size_t>,
public virtual constructor<size_t>, 
public virtual accessor<size_t>,
{
    public:
    using constructor<size_t>::constructor;
    using accessor<size_t>::get;

    using data<size_t>::bits;
    using data<size_t>::str;
};

// add mutator inheritance to create a mutable class
class MutableBitField : 
...
public virtual mutator<size_t>
{
    public:
    ...
    using mutator<size_t>::set;
    using mutator<size_t>::insert;
};
```

## example of usage

```c++
    MutableBitField x {};   // initialized to 0
    x.insert(
        {1,0,2,0,3},        // atomic bitwise data
        1,                  // starting index
        2                   // offset
        );                  // x value now equal to 0b11001000010
    
    cout << x.str();        // expect to print '11001000010'
```
