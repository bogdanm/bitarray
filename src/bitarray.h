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

#ifndef __BITARRAY_H__
#define __BITARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Configurable data:
// - BA_ELEMENT_TYPE_BITS: the size of the array's element as accepted by the get/set functions. This gives an upper
//   limit to the size of the elements in the array (for example, if BA_ELEMENT_TYPE_BITS is 16, the array can only
//   hold elements from one bit to 16 bits). Can be 8, 16, 32 or 64. Defaults to 32 if not specified.
// - BA_UNIT_SIZE_BITS: physical storage size in memory for the array (type of the pointer to the array)
//   Can be 8, 16, 32 or 64. Defaults to BA_ELEMENT_TYPE_BITS if not specified.
// - BA_ARRAY_SIZE_BITS: number of bits to represent the array *size*. Can be 8, 16, 32 or 64. Optional,
//   defaults to 32 if not specified.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Figure out the data types used by the array

// If BA_ELEMENT_TYPE_BITS is not defined, define it automatically
#ifndef BA_ELEMENT_TYPE_BITS
#warning Setting BA_ELEMENT_TYPE_BITS to the default value 32
#define BA_ELEMENT_TYPE_BITS 32
#endif // #ifndef BA_ELEMENT_TYPE_BITS

#if BA_ELEMENT_TYPE_BITS == 8
typedef uint8_t ba_element_t;
#elif BA_ELEMENT_TYPE_BITS == 16
typedef uint16_t ba_element_t;
#elif BA_ELEMENT_TYPE_BITS == 32
typedef uint32_t ba_element_t;
#elif BA_ELEMENT_TYPE_BITS == 64
typedef uint64_t ba_element_t;
#else
#error Invalid value for BA_ELEMENT_TYPE_BITS (must be 8, 16, 32 or 64)
#endif // #if BA_ELEMENT_TYPE_BITS == 8

// If BA_UNIT_SIZE_BITS is not defined, default to 8
#ifndef BA_UNIT_SIZE_BITS
#warning Setting BA_UNIT_SIZE_BITS to the value of BA_ELEMENT_TYPE_BITS
#define BA_UNIT_SIZE_BITS BA_ELEMENT_TYPE_BITS
#endif // #ifndef BA_UNIT_SIZE_BITS

#if BA_UNIT_SIZE_BITS == 8
typedef uint8_t ba_unit_t;
#elif BA_UNIT_SIZE_BITS == 16
typedef uint16_t ba_unit_t;
#elif BA_UNIT_SIZE_BITS == 32
typedef uint32_t ba_unit_t;
#elif BA_UNIT_SIZE_BITS == 64
typedef uint64_t ba_unit_t;
#else
#error Invalid value for BA_UNIT_SIZE_BITS (must be 8, 16, 32 or 64)
#endif // #if BA_UNIT_SIZE_BITS == 8

// Checks for BA_ARRAY_SIZE_BITS
#ifndef BA_ARRAY_SIZE_BITS
#warning Setting BA_ARRAY_SIZE_BITS to the default value 32
#define BA_ARRAY_SIZE_BITS 32
#endif // #ifndef BA_ARRAY_SIZE_BITS

#if BA_ARRAY_SIZE_BITS == 8
typedef uint8_t ba_size_t;
#elif BA_ARRAY_SIZE_BITS == 16
typedef uint16_t ba_size_t;
#elif BA_ARRAY_SIZE_BITS == 32
typedef uint32_t ba_size_t;
#elif BA_ARRAY_SIZE_BITS == 64
typedef uint64_t ba_size_t;
#else
#error Invalid value for BA_ARRAY_SIZE_BITS (must be 8, 16, 32 or 64)
#endif // #if BA_ARRAY_SIZE_BITS == 8

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Public interface

// This structure defines a bit array. It must be initialized first with 'ba_init', then it can be passed as the
// first parameter of 'ba_set' and 'ba_get' (below).
typedef struct {
    ba_unit_t* data;
    ba_size_t  size;
    uint8_t    element_size_bits;
} ba_array_t;

// Return the size (in *bytes*) of an array needed to keep "num_elements" elements of size "element_size_bits" each
#define BA_STORAGE_SIZE_BYTES(num, sz) ((((uint32_t)(num) * (sz) + 7) & (uint32_t)~7) >> 3)

// Initialize a bit array in "ba" with the given storage in "pbase" (which must be allocated before calling this
// function). The bit array will store "num_elements" elements of size "element_size_bits" each.
// NOTE: this function never fails. 'pbase' MUST point to a properly allocated memory space (see the
// BA_STORAGE_SIZE_BYTES macro above).
// The function returns its first parameter (ba)
ba_array_t *ba_init(ba_array_t *ba, ba_unit_t *pbase, ba_size_t num_elements, uint8_t element_size_bits);

// Sets 'data' at index 'idx' in the array at 'ba'. If idx is too large, it fails silently.
void ba_set(const ba_array_t *ba, ba_size_t idx, ba_element_t data);

// Returns the data at index 'idx' in the array at 'ba'. If the array is too large, it returns 0.
ba_element_t ba_get(const ba_array_t *ba, ba_size_t idx);

#ifdef __cplusplus
}
#endif

#endif // #ifndef __BITARRAY_H__
