//
// Created by Raul Romero on 2024-01-13.
//

#ifndef ATOM_DEFINES_H
#define ATOM_DEFINES_H

#include <cstdint>

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

#define ATOM_SUCCESS 0
#define ATOM_FAILURE 1
#define ATOM_ERROR -1

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#define ATOM_ATTEMPT(x) if (x != EXIT_SUCCESS) { return EXIT_FAILURE; }

#endif //ATOM_DEFINES_H
