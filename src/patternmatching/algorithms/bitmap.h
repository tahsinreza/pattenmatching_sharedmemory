
#ifndef PATTERNMATCHING_BITMAP_H
#define PATTERNMATCHING_BITMAP_H

#include <iostream>
#include <type_traits>
#include <iterator>

namespace patternmatching {

#define BITSET_DIV_ROUND_UP(X, Y) (X+Y-1)/Y

template<size_t MaxSize, typename Type, bool Const = true>
class BitsetIterator;

template<size_t MaxSize>
class FixedBitmap {
 public :
  using BitmapType = uint32_t;
  using const_iterator = BitsetIterator<MaxSize, const FixedBitmap *, true>;
  friend class BitsetIterator<MaxSize, const FixedBitmap *, true>;

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

  bool empty() const;
  size_t size() const;

  const_iterator find(const size_t &value) const;
  size_t count(const size_t &value) const;
  bool isIn(const size_t &value) const;

  void insert(const size_t &value);
  template <class InputIterator>
  void insert ( InputIterator first, InputIterator last );

  void erase(const size_t &value);
  void clear();
  void fill();

  const_iterator begin() const;
  const_iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;

 public:
  inline bool getBit(const size_t &value) const;
  inline size_t getBitmapIndex(const size_t &value) const;
  inline size_t getBitmapSubindex(const size_t &value) const;

  BitmapType map[C_BITMAP_SIZE];
};

template<size_t MaxSize>
typename FixedBitmap<MaxSize>::const_iterator begin(FixedBitmap<MaxSize>& obj);
template<size_t MaxSize>
typename FixedBitmap<MaxSize>::const_iterator end(FixedBitmap<MaxSize>& obj);


template<size_t MaxSize, typename Type, bool Const>
class BitsetIterator {
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
  BitsetIterator();
  BitsetIterator(Type pFixedBitset_);
  BitsetIterator(Type pFixedBitset_, size_t index_);
  BitsetIterator(const BitsetIterator<MaxSize, Type, false> &other);

  bool operator==(const BitsetIterator &other) const;
  bool operator!=(const BitsetIterator &other) const;

  reference operator*();

  BitsetIterator &operator--();
  BitsetIterator operator--(int);

  BitsetIterator &operator++();
  BitsetIterator operator++(int);

  friend class BitsetIterator<MaxSize, Type, true>;
};

}

#include "bitmap.tpp"
#endif //PATTERNMATCHING_BITSET_H
