#include "bitmap.h"
#include <bitset>

namespace patternmatching {

template<size_t MaxSize>
FixedBitmap<MaxSize>::FixedBitmap() : map{0} {};

template<size_t MaxSize>
FixedBitmap<MaxSize>::FixedBitmap(const FixedBitmap <MaxSize> &other) {
  for (int i = 0; i < C_BITMAP_SIZE; i++) {
    map[i] = other.map[i];
  }
};

template<size_t MaxSize>
void FixedBitmap<MaxSize>::print(std::ostream &ostream) const {
  bool first = true;
  for (const auto &it : *this) {
    if (!first) {
      ostream << " ";
    }
    ostream << it;
    first = false;
  }
  ostream << std::endl;
}
template<size_t MaxSize>
void FixedBitmap<MaxSize>::printRaw(std::ostream &ostream) const {
  bool first = true;
  for (size_t i = C_BITMAP_SIZE - 1; i > 0; i--) {
    if (!first) {
      ostream << " ";
    }
    ostream << std::bitset<C_BITS_PER_BITMAP>(map[i]);
    first = false;
  }
  if (!first) {
    ostream << " ";
  }
  ostream << std::bitset<C_BITS_PER_BITMAP>(map[0]);
  ostream << std::endl;
}

template<size_t MaxSize>
bool FixedBitmap<MaxSize>::empty() const {
  for (size_t i = 0; i < C_BITMAP_SIZE; i++) {
    if (map[i] != 0) return false;
  }
  return true;
};

template<size_t MaxSize>
size_t FixedBitmap<MaxSize>::size() const {
  size_t size = 0;
  for (size_t i = 0; i < C_BITMAP_SIZE; i++) {
    auto &value = map[i];
    size += __builtin_popcount(value);
  }
  return size;
};

template<size_t MaxSize>
typename FixedBitmap<MaxSize>::const_iterator FixedBitmap<MaxSize>::find(const size_t &value) const {
  if (map[getBitmapIndex(value)] & (1 << getBitmapSubindex(value))) return const_iterator(this, value);
  return 0;
}

template<size_t MaxSize>
size_t FixedBitmap<MaxSize>::count(const size_t &value) const {
  return static_cast<size_t>(getBit(value));
}

template<size_t MaxSize>
bool FixedBitmap<MaxSize>::isIn(const size_t &value) const {
  return getBit(value);
}

template<size_t MaxSize>
void FixedBitmap<MaxSize>::insert(const size_t &value) {
  map[getBitmapIndex(value)] |= 1 << getBitmapSubindex(value);
}

template<size_t MaxSize>
void FixedBitmap<MaxSize>::erase(const size_t &value) {
  map[getBitmapIndex(value)] &= ~(1 << getBitmapSubindex(value));
}
template<size_t MaxSize>
void FixedBitmap<MaxSize>::clear() {
  for (int i = 0; i < C_BITMAP_SIZE; i++) {
    map[i] = 0;
  }
}
template<size_t MaxSize>
void FixedBitmap<MaxSize>::fill() {
  for (int i = 0; i < C_BITMAP_SIZE; i++) {
    map[i] = static_cast<BitmapType>(~0);
  }
}

template<size_t MaxSize>
typename FixedBitmap<MaxSize>::const_iterator FixedBitmap<MaxSize>::cbegin() const {
  return const_iterator(this);
}

template<size_t MaxSize>
typename FixedBitmap<MaxSize>::const_iterator FixedBitmap<MaxSize>::cend() const {
  return const_iterator();
}
template<size_t MaxSize>
typename FixedBitmap<MaxSize>::const_iterator FixedBitmap<MaxSize>::begin() const {
  return const_iterator(this);
}

template<size_t MaxSize>
typename FixedBitmap<MaxSize>::const_iterator FixedBitmap<MaxSize>::end() const {
  return const_iterator();
}

template<size_t MaxSize>
typename FixedBitmap<MaxSize>::const_iterator begin(FixedBitmap <MaxSize> &obj) {
  return obj.begin();
}
template<size_t MaxSize>
typename FixedBitmap<MaxSize>::const_iterator end(FixedBitmap <MaxSize> &obj) {
  return obj.end();
}

template<size_t MaxSize>
bool FixedBitmap<MaxSize>::getBit(const size_t &value) const {
  /*std::cout << value << std::endl;
  std::cout << getBitmapIndex(value) << std::endl;
  std::cout << getBitmapSubindex(value) << std::endl;
  std::cout << (map[getBitmapIndex(value)] ) << std::endl;
  std::cout << ((1 << getBitmapSubindex(value))) << std::endl;
  std::cout << (map[getBitmapIndex(value)] & (1 << getBitmapSubindex(value))) << std::endl;*/
  return (map[getBitmapIndex(value)] & (1 << getBitmapSubindex(value))) > 0;
}

template<size_t MaxSize>
size_t FixedBitmap<MaxSize>::getBitmapIndex(const size_t &value) const {
  return value / C_BITS_PER_BITMAP;
}

template<size_t MaxSize>
size_t FixedBitmap<MaxSize>::getBitmapSubindex(const size_t &value) const {
  return value - getBitmapIndex(value) * C_BITS_PER_BITMAP;
}

template<size_t MaxSize, typename Type, bool Const>
BitsetIterator<MaxSize, Type, Const>::BitsetIterator() : pFixedBitset(nullptr), index(MaxSize) {};

template<size_t MaxSize, typename Type, bool Const>
BitsetIterator<MaxSize, Type, Const>::BitsetIterator(Type pFixedBitset_) : pFixedBitset(pFixedBitset_), index(0) {
  while (index < MaxSize) {
    if (pFixedBitset->getBit(index) == 0) index++;
    else return;
  }
  index = MaxSize;
  pFixedBitset = nullptr;
};

template<size_t MaxSize, typename Type, bool Const>
BitsetIterator<MaxSize, Type, Const>::BitsetIterator(Type pFixedBitset_, size_t index_)
    : pFixedBitset(pFixedBitset_), index(index_) {
  while (index < MaxSize) {
    if (pFixedBitset->getBit(index) == 0) index++;
    else return;
  }
  index = MaxSize;
  pFixedBitset = nullptr;
};

template<size_t MaxSize, typename Type, bool Const>
BitsetIterator<MaxSize, Type, Const>::BitsetIterator(const BitsetIterator<MaxSize, Type, false> &other)
    : pFixedBitset(other.pFixedBitset), index(other.index) {};

template<size_t MaxSize, typename Type, bool Const>
bool BitsetIterator<MaxSize, Type, Const>::operator==(const BitsetIterator &other) const {
  return (pFixedBitset == other.pFixedBitset) && (index == other.index);
}

template<size_t MaxSize, typename Type, bool Const>
bool BitsetIterator<MaxSize, Type, Const>::operator!=(const BitsetIterator &other) const {
  return (pFixedBitset != other.pFixedBitset) || (index != other.index);
}

template<size_t MaxSize, typename Type, bool Const>
typename BitsetIterator<MaxSize, Type, Const>::reference BitsetIterator<MaxSize, Type, Const>::operator*() {
  return index;
}

template<size_t MaxSize, typename Type, bool Const>
BitsetIterator<MaxSize, Type, Const> &BitsetIterator<MaxSize, Type, Const>::operator--() {
  do {
    index--;
    if (pFixedBitset->getBit(index)) return *this;
  } while (index > 0);
  index = MaxSize;
  pFixedBitset = nullptr;
  return *this;
}

template<size_t MaxSize, typename Type, bool Const>
BitsetIterator<MaxSize, Type, Const> BitsetIterator<MaxSize, Type, Const>::operator--(int) {
  const BitsetIterator old(*this);
  --(*this);
  return old;
}

template<size_t MaxSize, typename Type, bool Const>
BitsetIterator<MaxSize, Type, Const> &BitsetIterator<MaxSize, Type, Const>::operator++() {
  do {
    index++;
    if (pFixedBitset->getBit(index)) return *this;
  } while (index < MaxSize - 1);
  index = MaxSize;
  pFixedBitset = nullptr;
  return *this;
}

template<size_t MaxSize, typename Type, bool Const>
BitsetIterator<MaxSize, Type, Const> BitsetIterator<MaxSize, Type, Const>::operator++(int) {
  // Use operator++()
  const BitsetIterator old(*this);
  ++(*this);
  return old;
}

}