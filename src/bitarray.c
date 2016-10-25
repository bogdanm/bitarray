/*
 * Copyright 2016 Bogdan Marinescu.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <inttypes.h>
#include "bitarray.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Internal functions and helpers

#define BA_MIN(x, y) ((x) < (y) ? (x) : (y))

typedef struct {
    ba_size_t idx;        // index in the array
    uint8_t element_bits; // size of one element in bits
    uint8_t left_bits;    // how much bits are left to process
    uint8_t bit_pos;      // current bit position
    ba_element_t mask;    // mask for current elements
    uint8_t st_shift;     // shift for mask in storage
} bitdesc_t;

// Compute the required masks and shifts based on the bitdesc_t structure. Return 1 if there's more
// work to be done, 0 otherwise.
static int compute_next(bitdesc_t *pdesc) {
    uint32_t temp32;
    uint8_t crt_bits;

    if (pdesc->left_bits == 0) // all done
        return 0;
    if (pdesc->element_bits == pdesc->left_bits) { // first iteration, compute bit position and index
        temp32 = (uint32_t)pdesc->idx * pdesc->element_bits;
        // After the next statement, 'idx' will contain the *physical* index in the storage
        pdesc->idx = (ba_size_t)(temp32 / BA_UNIT_SIZE_BITS);
        pdesc->bit_pos = (uint8_t)(BA_UNIT_SIZE_BITS - 1 - (temp32 % BA_UNIT_SIZE_BITS));
    } else if (pdesc->bit_pos == BA_UNIT_SIZE_BITS - 1) {
        // Need to advance to the next index
        pdesc->idx ++;
    }
    // How much bits can be processed in this iteration?
    crt_bits = BA_MIN(pdesc->bit_pos + 1, pdesc->left_bits);
    // Compute mask for "crt_bits"
    pdesc->mask = ((ba_unit_t)1 << crt_bits) - 1;
    if (pdesc->mask == 0) // this must be an overflow on the 'ba_unit_t' type, so recompute
        pdesc->mask = (ba_element_t)~0;
    // Compute shifts for the storage
    pdesc->st_shift = pdesc->bit_pos + 1 - crt_bits;
    // Update remaining bits
    pdesc->left_bits -= crt_bits;
    // And update bit position
    if (pdesc->bit_pos >= pdesc->element_bits)
        pdesc->bit_pos -= pdesc->element_bits;
    else
        pdesc->bit_pos = BA_UNIT_SIZE_BITS - 1;
    return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Public interface

ba_array_t *ba_init(ba_array_t *ba, ba_unit_t *pbase, ba_size_t num_elements, uint8_t element_size_bits) {
    ba->data = pbase;
    ba->size = num_elements;
    ba->element_size_bits = element_size_bits;
    return ba;
}

void ba_set(const ba_array_t *ba, ba_size_t idx, ba_element_t data) {
    bitdesc_t bd = {idx, ba->element_size_bits, ba->element_size_bits, 0, 0, 0};
    ba_unit_t t, e;

    if (idx >= ba->size)
        return;
    while (compute_next(&bd)) {
        // Read whole element in the storage
        t = ba->data[bd.idx];
        // Mask out the parts the won't change
        t = t & (ba_unit_t)~((ba_unit_t)bd.mask << bd.st_shift);
        // Get the relevant part of 'data'
        e = (ba_unit_t)((data >> bd.left_bits) & bd.mask);
        // Write back data (properly shifted)
        ba->data[bd.idx] = t | (e << bd.st_shift);
    }
}

ba_element_t ba_get(const ba_array_t *ba, ba_size_t idx) {
    bitdesc_t bd = {idx, ba->element_size_bits, ba->element_size_bits, 0, 0, 0};
    ba_element_t data = 0;
    ba_unit_t t;

    if (idx >= ba->size)
        return 0;
    while (compute_next(&bd)) {
        // Read whole element in the storage
        t = ba->data[bd.idx];
        // Mask out unneeded parts
        t = (t >> bd.st_shift) & bd.mask;
        // Update data
        data = data | ((ba_element_t)t << bd.left_bits);
    }
    return data;
}

#ifdef TESTME
uint64_t storage[1000];

int main() {
    ba_array_t ba;

    ba_init(&ba, storage, 22, 3);
    ba_set(&ba, 0, 6);
    printf("%016llX\n", storage[0]);
    ba_set(&ba, 10, 3);
    printf("%016llX\n", storage[0]);
    printf("%d\n", ba_get(&ba, 0));
    return 0;
    for (unsigned j = 0; j < 22; j ++)
    {
        printf("*** SET %u\n", j);
        ba_set(&ba, j, j % 7);
        for (unsigned i = 0; i < 22; i ++) {
            printf("[%u]=%d ", (unsigned)i, ba_get(&ba, i));
            if (i == 21)
                printf("\n");
        }
    }
}
#endif

