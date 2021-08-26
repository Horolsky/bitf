# BITF DEVELOPMENT ROADMAP

## Idea  
Granular bitwise operations on bitfields:  
- scalar and vector bitfield data storage of adjustable size, both static and dynamic;  
- get/set the scalar value of adjustable bit indent and offset;  
- get/set the vector of values of adjustable bit indent and continuous offset;  
- low level raw functions + OOP & FP wrappings  

## Structure  
Current working structure:  
- **solid** namespace: scalar storage  
    - [x] raw functions without overflow check (noexcept)  
    - [] FP wrappings  
    - [x] OOP wrappings: **cls** namespace (*in progress*)  
- **chunked** namespace: vector storage  
    - [] raw functions (*few examples completed*)  
    - [x] **_static** storage  
    - [x] **_dynamic** storage  
    - [] FP wrappings  
    - [] OOP wrappings  
 
 ## API concepts  
raw functions are error prone, all runtime checks delegated to their clients.  
common template parameters for raw functions:  
 - bitfield storage type (unsigned integer)  
 - value type for scalar getters/setters,   
 - collection iterator/pointer type for bulk getters/setters  
keeping bitfield type parameter at the end of the parameter list allows the compiler to determine its type implicitly from the function arguments

 
order of arguments for raw functions:
```c++
context args..., BitfieldType bits, size_t offset, size_t indent
```



