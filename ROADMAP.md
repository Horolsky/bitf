# BITF DEVELOPMENT ROADMAP

## Idea
Granular bitwise operations on bitfields:
- adjustable bitfield data storage size, both static and dynamic;
- get/set the scalar value of adjustable bit offset;
- get/set the vector of values of adjustable continuous bit offset;
- both functional and objective oriented implementations;

## Stage 1
- [x] clean functional implementation wrapped in a template static class that operates on a primitive type;

## Stage 2
- [x] OOP wrappers that follow the separation of concerns:
    - [x] raw bitfield data storage in primitive type;
    - [x] granular bitfield constructor, accessor and mutator that provides an interface to data. 

## Stage 3
- Separation of data storage and interfaces through the pure abstract classes;
- Data storage wrapper in 2 versions:
    - static array of primitives (this will override the Stage 1 version);
    - dynamic array of primitives;
- Remove control on the primitive type from the data storage template, use the bit size as a template parameter instead;