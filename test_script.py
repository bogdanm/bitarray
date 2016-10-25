import os, sys

bit_sizes = [8, 16, 32, 64]

for unit_size in bit_sizes:
    for element_size in bit_sizes:
        print "---------------- Testing unit size=%d, element size=%d" % (unit_size, element_size)
        res = os.system("gcc -std=c99 -m32 -Os -DBA_UNIT_SIZE_BITS=%d -DBA_ELEMENT_TYPE_BITS=%d -DBA_ARRAY_SIZE_BITS=32 bitarray.c bitarray_test.c -o bitarray_test.elf" % (unit_size, element_size))
        if res != 0:
            sys.exit(1)
        res = os.system("./bitarray_test.elf")
        if res != 0:
            sys.exit(2)
