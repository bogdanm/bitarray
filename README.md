# Bit array

The bit array is an array that holds elements of equal size. Unlike a regular (C/C++) array, its elements can have any size between 1 bit and 64 bits (see the next paragraph for more details). The array's elements are contiguous in memory, so an array of 10 3-bit elements will need 4 bytes of storage (rounded up from 30 bits to a multiple of 8 bits). This makes the bit array useful for applications where lots of elements of unusual size (not a multiple of 8 bits) need to be packed efficiently.

## Using

There are only 3 functions in the library:

- `ba_init` initializes a bit array.
- `ba_set` sets an element in the bit array.
- `ba_get` gets an element from the bit array.

The full documentation can be found in `bitarray.h`, but an example should be enough to understand the library:

```
#include <stdio.h>
#include <stdint.h>
#include "bitarray.h"

int main() {
    // Allocate storage for 8 elements of 3 bits each.
    // The macro BA_STORAGE_SIZE_BYTES computes the required storage in bytes.
    uint8_t storage[BA_STORAGE_SIZE_BYTES(8, 3)];
    unsigned i;
    ba_array_t ba; // this is the array descriptor

    printf("The array size is %u bytes.\n", sizeof(storage)); // 3 bytes are used for this bit array
    // Initialize bit array with physical storage in 'storage', 8 elements, 3 bits/element
    ba_init(&ba, (ba_unit_t*)storage, 8, 3);
    for (i = 0; i < 8; i ++)
        ba_set(&ba, i, 7 - i); // set the element in bit array at address 'i' to '7-i'
    // After the loop above, the bit array contains 7 6 5 4 3 2 1 0 (remember that each element is 3 bits in size).
    // Verify array content
    for (i = 0; i < 8; i ++)
        printf("bit_array[%u] = %u\n", i, ba_get(&ba, i));
    return 0;
}
```

## Configuring

It's not strictly required to configure the library, as it comes with defaults that should work for the most common cases. If the defaults are not satisfactory, there are 3 macros that can be used to configure the implementation at compile time:

1. `BA_ELEMENT_TYPE_BITS` defines the type used to access the data in the bit array (elements of this type are returned by `ba_get` and set with `ba_set`). The size of one element in the bit array can be:

    - 1 to 8 bits if `BA_ELEMENT_TYPE_BITS` is 8.
    - 1 to 16 bits if `BA_ELEMENT_TYPE_BITS` is 16.
    - 1 to 32 bits if `BA_ELEMENT_TYPE_BITS` is 32.
    - 1 to 64 bits if `BA_ELEMENT_TYPE_BITS` is 64.

If not specified, `BA_ELEMENT_TYPE_BITS` defaults to 32.

2. `BA_UNIT_SIZE_BITS` defines the type of the array's **unit**. The bit array has a storage which is itself an array of elements of `BA_UNIT_SIZE_BITS` in size, these elements are called **units**. This parameter has an effect on the implementation, not on the size of the elements that can be stored in the array (which is defined by `BA_ELEMENT_TYPE_BITS` above). It's recommended to set this parameter to the size of `int` (32 for a 32-bit CPU, 8 (or 16) for an 8-bit CPU and so on). If not specified, `BA_UNIT_SIZE_BITS` defaults to the value of `BA_ELEMENT_TYPE_BITS`.

3. `BA_ARRAY_SIZE_BITS` defines a type that's used to specify the **size** of the bit array (the total number of elements that can fit in the array). If not specified, it defaults to 32. You probably won't have to change this parameter, unless you're using this library with very small, resource constrained MCUs.

`BA_ELEMENT_TYPE_BITS` and `BA_UNIT_SIZE_BITS` are not related. It's perfectly OK to define a bit array that uses a byte storage internally (`BA_UNIT_SIZE_BITS` is 8), but can store elements up to 64 bits in size (`BA_ELEMENT_TYPE_BITS` is 64). It's also perfectly OK (but probably not very useful) to define a bit array that uses a 64-bit storage internally (`BA_UNIT_SIZE_BITS` is 64), but can only store elements up to 8 bits in size (`BA_ELEMENT_TYPE_BITS` is 8).

## Limitations

- The array doesn't grow (the size is fixed).
- The bounds are checked, but if the check fails, there are no errors reported: `ba_set` fails silently, while `ba_get` returns 0.
- The implementation is not optimized for speed. For example, if you need an array that holds 1-bit values, a hand-crafted implementation (or a specialized library) will likely be much faster. On the other hand, if speed is not an issue, this implementation will work perfectly fine for 1-bit values.

## License

The code is licensed under the Apache 2.0 license.
