
#ifndef PATTERNMATCHING_BITMAP_FIXED_H
#define PATTERNMATCHING_BITMAP_FIXED_H

#include <iostream>
#include <type_traits>
#include <iterator>
#include "backward_compatibility.h"

#define BITSET_DIV_ROUND_UP(X, Y) ((X+Y-1)/(Y))

namespace patternmatching {


template<size_t MaxSize, typename Type, bool Const = true>
class FixedBitmapIterator;

template<size_t MaxSize>
class FixedBitmap {
 public :
  using BitmapType = uint32_t;
  using const_iterator = FixedBitmapIterator<MaxSize, const FixedBitmap *, true>;
  friend class FixedBitmapIterator<MaxSize, const FixedBitmap *, true>;

  static const size_t C_BITS_PER_BYTE = 8;
  static const size_t C_BYTE_PER_BITMAP = sizeof(BitmapType);
  static const size_t C_BITS_PER_BITMAP = C_BYTE_PER_BITMAP * C_BITS_PER_BYTE;

  static const size_t C_BITS_SIZE = MaxSize;
  static const size_t C_BYTE_SIZE = BITSET_DIV_ROUND_UP(C_BITS_SIZE, C_BITS_PER_BYTE);
  static const size_t C_BITMAP_SIZE = BITSET_DIV_ROUND_UP(C_BYTE_SIZE, C_BYTE_PER_BITMAP);

 public:
  FixedBitmap();
  FixedBitmap(const FixedBitmap &other);

  void print(std::ostream &ostream = std::cout) const;
  void printRaw(std::ostream &ostream = std::cout) const;

  inline bool empty() const;
  inline size_t size() const;

  inline const_iterator find(const size_t &value) const;
  inline size_t count(const size_t &value) const;
  inline bool isIn(const size_t &value) const;
  inline bool isInAtomic(const size_t &value) const;

  inline void insert(const size_t &value);
  inline void insertAtomic(const size_t &value);
  template <class InputIterator>
  inline void insert ( InputIterator first, InputIterator last );

  inline void erase(const size_t &value);
  inline void clear();
  inline void fill();

  inline const_iterator begin() const;
  inline const_iterator end() const;
  inline const_iterator cbegin() const;
  inline const_iterator cend() const;

 public:
  inline bool getBit(const size_t &value) const;
  inline bool getBitAtomic(const size_t &value) const;
  inline size_t getBitmapIndex(const size_t &value) const;
  inline size_t getBitmapSubindex(const size_t &value) const;

  BitmapType map[C_BITMAP_SIZE];
};

template<size_t MaxSize>
inline typename FixedBitmap<MaxSize>::const_iterator begin(FixedBitmap<MaxSize>& obj);
template<size_t MaxSize>
inline typename FixedBitmap<MaxSize>::const_iterator end(FixedBitmap<MaxSize>& obj);


template<size_t MaxSize, typename Type, bool Const>
class FixedBitmapIterator {
 public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = size_t;
  using difference_type = std::ptrdiff_t;
  using reference = typename std::conditional_t<Const, const size_t &, size_t &>;
  using pointer = typename std::conditional_t<Const, const size_t *, size_t *>;

 protected:
  Type pFixedBitset;
  value_type index;
 public:
  FixedBitmapIterator();
  FixedBitmapIterator(Type pFixedBitset_);
  FixedBitmapIterator(Type pFixedBitset_, size_t index_);
  FixedBitmapIterator(const FixedBitmapIterator<MaxSize, Type, false> &other);

  bool operator==(const FixedBitmapIterator &other) const;
  bool operator!=(const FixedBitmapIterator &other) const;

  reference operator*();

  FixedBitmapIterator &operator--();
  FixedBitmapIterator operator--(int);

  FixedBitmapIterator &operator++();
  FixedBitmapIterator operator++(int);

  friend class FixedBitmapIterator<MaxSize, Type, true>;
};

}

#include "bitmap_fixed.tpp"
#endif //PATTERNMATCHING_BITSET_H
