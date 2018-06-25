
#ifndef PATTERNMATCHING_BITMAP_DYNAMIC_H
#define PATTERNMATCHING_BITMAP_DYNAMIC_H

#include <iostream>
#include <type_traits>
#include <iterator>
#include "backward_compatibility.h"
#include "bitmap_fixed.h"

namespace patternmatching {


template<typename Type, bool Const = true>
class DynamicBitmapIterator;

class DynamicBitmap {
 public :
  using BitmapType = uint32_t;
  using const_iterator = DynamicBitmapIterator<const DynamicBitmap *, true>;
  friend class DynamicBitmapIterator<const DynamicBitmap *, true>;

  static const size_t C_BITS_PER_BYTE = 8;
  static const size_t C_BYTE_PER_BITMAP = sizeof(BitmapType);
  static const size_t C_BITS_PER_BITMAP = C_BYTE_PER_BITMAP * C_BITS_PER_BYTE;

  size_t bitsSize;
  size_t byteSize;
  size_t bitmapSize;

 public:
  DynamicBitmap();
  DynamicBitmap(const DynamicBitmap &other);
  ~DynamicBitmap();
  void allocate(const size_t &bitsSize);
  void free();

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
  inline void eraseAtomic(const size_t &value);
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

  BitmapType* map;
};

inline DynamicBitmap::const_iterator begin(DynamicBitmap& obj);
inline DynamicBitmap::const_iterator end(DynamicBitmap& obj);


template<typename Type = const DynamicBitmap *, bool Const>
class DynamicBitmapIterator {
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
  DynamicBitmapIterator();
  DynamicBitmapIterator(Type pFixedBitset_);
  DynamicBitmapIterator(Type pFixedBitset_, size_t index_);
  DynamicBitmapIterator(const DynamicBitmapIterator<Type, false> &other);

  bool operator==(const DynamicBitmapIterator &other) const;
  bool operator!=(const DynamicBitmapIterator &other) const;

  reference operator*();

  DynamicBitmapIterator &operator--();
  DynamicBitmapIterator operator--(int);

  DynamicBitmapIterator &operator++();
  DynamicBitmapIterator operator++(int);

  friend class DynamicBitmapIterator<Type, true>;
};

}

#include "bitmap_dynamic.tpp"
#endif //PATTERNMATCHING_BITSET_H
