#include "bitmap_dynamic.h"
#include <bitset>

namespace patternmatching {

bool DynamicBitmap::empty() const {
  for (size_t i = 0; i < bitmapSize; i++) {
    if (map[i] != 0) return false;
  }
  return true;
};

size_t DynamicBitmap::size() const {
  size_t size = 0;
  for (size_t i = 0; i < bitmapSize; i++) {
    auto &value = map[i];
    size += __builtin_popcount(value);
  }
  return size;
};

DynamicBitmap::const_iterator DynamicBitmap::find(const size_t &value) const {
  if (map[getBitmapIndex(value)] & (static_cast<BitmapType>(1) << getBitmapSubindex(value)))
    return const_iterator(this,
                          value);
  return cend();
}

size_t DynamicBitmap::count(const size_t &value) const {
  return static_cast<size_t>(getBit(value));
}

bool DynamicBitmap::isIn(const size_t &value) const {
  return getBit(value);
}

bool DynamicBitmap::isInAtomic(const size_t &value) const {
  return getBitAtomic(value);
}

void DynamicBitmap::insert(const size_t &value) {
  map[getBitmapIndex(value)] |= static_cast<BitmapType>(1) << getBitmapSubindex(value);
}
void DynamicBitmap::insertAtomic(const size_t &value) {
  BitmapType mask = static_cast<BitmapType>(1) << getBitmapSubindex(value);
  auto addr = &(map[getBitmapIndex(value)]);
  __sync_fetch_and_or(addr, mask);
}

template<class InputIterator>
void DynamicBitmap::insert(InputIterator first, InputIterator last) {
  for (auto it = first; it != last; ++it) {
    insert(*it);
  }
}



void DynamicBitmap::erase(const size_t &value) {
  map[getBitmapIndex(value)] &= ~(static_cast<BitmapType>(1) << getBitmapSubindex(value));
}
void DynamicBitmap::clear() {
  for (int i = 0; i < bitmapSize; i++) {
    map[i] = 0;
  }
}
void DynamicBitmap::fill() {
  for (int i = 0; i < bitmapSize; i++) {
    map[i] = ~static_cast<BitmapType>(0);
  }
}

typename DynamicBitmap::const_iterator DynamicBitmap::cbegin() const {
  return const_iterator(this);
}

typename DynamicBitmap::const_iterator DynamicBitmap::cend() const {
  return const_iterator();
}
typename DynamicBitmap::const_iterator DynamicBitmap::begin() const {
  return const_iterator(this);
}

typename DynamicBitmap::const_iterator DynamicBitmap::end() const {
  return const_iterator();
}

typename DynamicBitmap::const_iterator begin(DynamicBitmap &obj) {
  return obj.begin();
}

typename DynamicBitmap::const_iterator end(DynamicBitmap &obj) {
  return obj.end();
}

bool DynamicBitmap::getBit(const size_t &value) const {
  return (map[getBitmapIndex(value)] & (1 << getBitmapSubindex(value))) > 0;
}

bool DynamicBitmap::getBitAtomic(const size_t &value) const {
  BitmapType mapValue;
  auto addr = &(map[getBitmapIndex(value)]);
  #pragma omp atomic read
  mapValue = *addr;
  return (mapValue & (static_cast<BitmapType>(1) << getBitmapSubindex(value))) > 0;
}

size_t DynamicBitmap::getBitmapIndex(const size_t &value) const {
  return value / C_BITS_PER_BITMAP;
}

size_t DynamicBitmap::getBitmapSubindex(const size_t &value) const {
  return value - getBitmapIndex(value) * C_BITS_PER_BITMAP;
}

template<typename Type, bool Const>
DynamicBitmapIterator<Type, Const>::DynamicBitmapIterator() : pFixedBitset(nullptr), index(0) {};

template<typename Type, bool Const>
DynamicBitmapIterator<Type, Const>::DynamicBitmapIterator(Type pFixedBitset_) : pFixedBitset(pFixedBitset_), index(0) {
  while (index < pFixedBitset->bitsSize) {
    if (pFixedBitset->getBit(index) == 0) index++;
    else return;
  }
  index = 0;
  pFixedBitset = nullptr;
};

template<typename Type, bool Const>
DynamicBitmapIterator<Type, Const>::DynamicBitmapIterator(Type pFixedBitset_, size_t index_)
    : pFixedBitset(pFixedBitset_), index(index_) {
  while (index < pFixedBitset->bitsSize) {
    if (pFixedBitset->getBit(index) == 0) index++;
    else return;
  }
  index = 0;
  pFixedBitset = nullptr;
};

template<typename Type, bool Const>
DynamicBitmapIterator<Type, Const>::DynamicBitmapIterator(const DynamicBitmapIterator<Type, false> &other)
    : pFixedBitset(other.pFixedBitset), index(other.index) {};

template<typename Type, bool Const>
bool DynamicBitmapIterator<Type, Const>::operator==(const DynamicBitmapIterator &other) const {
  return (pFixedBitset == other.pFixedBitset) && (index == other.index);
}

template<typename Type, bool Const>
bool DynamicBitmapIterator<Type, Const>::operator!=(const DynamicBitmapIterator &other) const {
  return (pFixedBitset != other.pFixedBitset) || (index != other.index);
}

template<typename Type, bool Const>
typename DynamicBitmapIterator<Type, Const>::reference DynamicBitmapIterator<Type, Const>::operator*() {
  return index;
}

template<typename Type, bool Const>
DynamicBitmapIterator <Type, Const> &DynamicBitmapIterator<Type, Const>::operator--() {
  do {
    index--;
    if (pFixedBitset->getBit(index)) return *this;
  } while (index > 0);
  index = 0;
  pFixedBitset = nullptr;
  return *this;
}

template<typename Type, bool Const>
DynamicBitmapIterator <Type, Const> DynamicBitmapIterator<Type, Const>::operator--(int) {
  const DynamicBitmapIterator old(*this);
  --(*this);
  return old;
}

template<typename Type, bool Const>
DynamicBitmapIterator <Type, Const> &DynamicBitmapIterator<Type, Const>::operator++() {
  do {
    index++;
    if (pFixedBitset->getBit(index)) return *this;
  } while (index < pFixedBitset->bitsSize - 1);
  index = 0;
  pFixedBitset = nullptr;
  return *this;
}

template<typename Type, bool Const>
DynamicBitmapIterator <Type, Const> DynamicBitmapIterator<Type, Const>::operator++(int) {
  // Use operator++()
  const DynamicBitmapIterator old(*this);
  ++(*this);
  return old;
}

}