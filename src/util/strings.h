//
// Created by Stanislav Bashkyrtsev on 9/4/25.
//

#ifndef CMAKE_PET_STRINGS_H
#define CMAKE_PET_STRINGS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 *
 * @param dst pointer to copy the string to
 * @param append which string to point (must end with \0)
 * @param dstLen what's the current length of the dst string (including \0)
 * @param appendLen how many symbols are in append string (including \0)
 * @return potentially new pointer to dst, the old one may be freed depending on the results of realloc()
 */
char* strconcat(char* dst, const char* append, size_t dstLen, size_t appendLen);

#endif //CMAKE_PET_STRINGS_H