//
// Created by qiu on 21/06/18.
//

#include "multiple_label_constraint_effectiveness.h"
#include <cmath>

namespace patternmatching {

template<typename T>
T MultipleLabelConstraintEffectiveness::averageDifferentVertexFromTriesApproximateStrong(const T vertexNumber,
                                                                                   const T tries) const {
  return (tries <= vertexNumber) ? tries : vertexNumber;
}

template<typename T>
T MultipleLabelConstraintEffectiveness::averageDifferentVertexFromTriesApproximate(const T vertexNumber,
                                                                        const T tries) const {
  T res=0;
  for(T uniqueVertex=1; uniqueVertex<=vertexNumber;uniqueVertex++) {
    res+=uniqueVertex*probaDifferentVertexFromTriesApproximate(vertexNumber, tries, uniqueVertex);
  }
  return res;
}

template<typename T>
T MultipleLabelConstraintEffectiveness::averageDifferentVertexFromTries(const T vertexNumber,
                                                                        const T tries) const {
  T res=0;
  for(T uniqueVertex=0; uniqueVertex<=vertexNumber;uniqueVertex++) {
    res+=uniqueVertex*probaDifferentVertexFromTries(vertexNumber, tries, uniqueVertex);
  }
  return res;
}

template<typename T>
T MultipleLabelConstraintEffectiveness::probaDifferentVertexFromTriesApproximate(const T vertexNumber,
                                                                                      const T tries,
                                                                                      const T uniqueVertex) const {
  if(uniqueVertex<=0 || uniqueVertex>vertexNumber) return 0;
  auto probaNotPicked = pow(static_cast<double>(vertexNumber - 1) / vertexNumber, tries);
  auto probaPicked = 1 - probaNotPicked;
  return ncr(vertexNumber, uniqueVertex) * (pow(1 - probaPicked, vertexNumber - uniqueVertex))
      * (pow(probaPicked, uniqueVertex));
}

template<typename T>
T MultipleLabelConstraintEffectiveness::probaDifferentVertexFromTries(const T vertexNumber,
                                                                      const T tries,
                                                                      const T uniqueVertex) const {

  if (uniqueVertex <= 0 || uniqueVertex > vertexNumber) return 0;
  return static_cast<double>(ncr(vertexNumber, uniqueVertex) * factorial(uniqueVertex) * stirling2(tries, uniqueVertex))
      / pow(vertexNumber, tries);

}

template<typename T>
T MultipleLabelConstraintEffectiveness::factorial(const T n) const {
  T res=1;
  for(auto k=1;k<n;k++) {
    res*=k;
  }
  return res;
}

template<typename T>
T MultipleLabelConstraintEffectiveness::stirling2(const T n, const T k) const {
  T maxj = n-k;

  T arr = new T[maxj+1];

  for (int i = 0; i <= maxj; ++i)
    arr[i] = 1;

  for (int i = 2; i <= k; ++i)
    for(int j = 1; j <= maxj; ++j)
      arr[j] += i*arr[j-1];

  T res=arr[maxj];
  delete[] arr;
  return res;
}

template<typename T>
T MultipleLabelConstraintEffectiveness::ncr(const T n, const T k) const {
/// A fast way to calculate binomial coefficients by Andrew Dalke (contrib).
  if (k < 0 || k > n) return 0;
  T ncpy = n;
  T ntok = 1;
  T ktok = 1;
  T minKN = (k < n - k) ? k : n - k;
  for (size_t t = 1; t < minKN + 1; t++) {
    ntok *= ncpy;
    ktok *= t;
    ncpy -= 1;
  }
  return ntok; // ktok
}


}