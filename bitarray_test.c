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

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>
#include "bitarray.h"

// 10k of memory for tests
static ba_unit_t storage[10000 * 8 / BA_UNIT_SIZE_BITS];

// Returns 0 for OK, 1 for error
static int testit(ba_size_t num_elements, uint8_t element_size, unsigned passes, int verbose) {
    uint64_t *exp = NULL, t64;
    unsigned pass;
    ba_size_t i;
    ba_element_t t;
    int res = 0;
    ba_array_t ba;

    printf("*** TEST num_elements=%u, element_size=%u, passes=%u ... ", (unsigned)num_elements, (unsigned)element_size, (unsigned)passes);
    if (BA_STORAGE_SIZE_BYTES(num_elements, element_size) > sizeof(storage)) {
        printf("not enough storage! ");
        res = 1;
        goto done;
    }
    if ((exp = (uint64_t*)malloc(num_elements * sizeof(uint64_t))) == NULL) {
        printf("not enough memory! ");
        res = 1;
        goto done;
    }
    ba_init(&ba, storage, num_elements, element_size);
    for (pass = 0; pass < passes; pass ++) {
        srand(time(NULL));
        // Fill the array forward
        for (i = 0; i < num_elements; i ++) {
            if (i == 0) // force lowest value
                t64 = 0;
            else if (i == num_elements - 1) // force highest value
                t64 = ((uint64_t)1 << element_size) - 1;
            else
                t64 = (uint64_t)rand() % ((uint64_t)1 << element_size);
            exp[i] = t64;
            ba_set(&ba, i, (ba_element_t)t64);
        }
        // Read back
        for (i = 0; i < num_elements; i ++) {
            t = ba_get(&ba, i);
            if ((uint64_t)t != exp[i]) {
                res = 1;
                if (verbose)
                    printf("ERROR (forward) at index %u: expected %" PRIu64 ", got %" PRIu64 "\n", (unsigned)i, (uint64_t)exp[i], (uint64_t)t);
            }
        }
        // Fill the array backward
        for (i = num_elements; i > 0; i --) {
            if (i == num_elements) // force lowest value
                t64 = 0;
            else if (i == 1) // force highest value
                t64 = ((uint64_t)1 << element_size) - 1;
            else
                t64 = (uint64_t)rand() % ((uint64_t)1 << element_size);
            exp[i - 1] = t64;
            ba_set(&ba, i - 1, (ba_element_t)t64);
        }
        // Read back
        for (i = num_elements; i > 0; i --) {
            t = ba_get(&ba, i - 1);
            if ((uint64_t)t != exp[i - 1]) {
                res = 1;
                if (verbose)
                    printf("ERROR (backward) at index %u: expected %" PRIu64 ", got %" PRIu64 "\n", (unsigned)i - 1, (uint64_t)exp[i - 1], (uint64_t)t);
            }
        }
    }
done:
    printf("%s\n", res ? "FAIL" : "PASS");
    if (exp)
        free(exp);
    return res;
}

int main() {
    uint8_t s;
    int res = 0;

    // Test all possible sizes of elements in the bit array
    for (s = 1; s <= BA_ELEMENT_TYPE_BITS; s ++)
        res = res | testit(1000, s, 1000, 1);
    return res;
}
