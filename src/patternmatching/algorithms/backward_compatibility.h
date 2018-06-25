//
// Created by qiu on 19/06/18.
//

#ifndef PROJECT_BACKWARD_COMPATIBILITY_H
#define PROJECT_BACKWARD_COMPATIBILITY_H
#include <type_traits>
#include <iterator>

#if __cplusplus<201402L
namespace std {
template<bool B, class T, class F>
using conditional_t = typename conditional<B, T, F>::type;
}
#endif

#endif //PROJECT_BACKWARD_COMPATIBILITY_H
