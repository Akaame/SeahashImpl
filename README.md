# A Dummy Implementation of Seahash Algorithm

This is a translation of the original Rust implementation of non-cryptographic hashing algorithm called Seahash.

### v1

v1 is the translation of the reference implementation which is easier to read.

### v2

v2 contains a faster implementation to the classical loop unrolling methodology. It strides forward 32 bytes in each iteration each one having four operations on four different inner states. Buffer implementation assures assures that the chunks read contain 8 byte integers and reinterpret casts them to uint64_t. For the excessive parts that are the remainders of 32 byte strides, possible scenarios are manually typed.

## Performance

Below results are from a 1,4 GHz Quad-Core Intel Core i5.

v1 Speed: 2.057 GB/s
v2 Speed: 3.508 GB/s

## TODOS

* Get rid of the part which creates larger strings concatting the same string into sstream and use CPP2A ranges.