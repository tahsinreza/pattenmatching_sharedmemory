//
// Created by qiu on 07/06/18.
//

#ifndef PROJECT_MULTIPLE_LABEL_COMMON_H
#define PROJECT_MULTIPLE_LABEL_COMMON_H

#include "totem_comdef.h"
#include "bitmap_fixed.h"
#include "common_utils.h"
namespace patternmatching {

typedef uint32_t pvid_t;
static const size_t C_MAXIMUM_PATTERN_SIZE = 64;
using FixedBitmapType = FixedBitmap<C_MAXIMUM_PATTERN_SIZE>;

#define DEBUG_PRINT(X) std::cout << #X<< "\t" << X <<std::endl;
#define DEBUG_PRINT_SET(X) \
std::cout<< #X << " : ";\
for(const auto &it : X) { std::cout << it << " ";} std::cout << std::endl;

}

#endif //PROJECT_MULTIPLE_LABEL_COMMON_H
