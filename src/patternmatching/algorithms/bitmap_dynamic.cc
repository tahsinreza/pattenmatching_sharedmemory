#include "bitmap_dynamic.h"
#include <bitset>

namespace patternmatching {


DynamicBitmap::DynamicBitmap() : bitsSize(0), byteSize(0), bitmapSize(0), map(nullptr){};

DynamicBitmap::DynamicBitmap(const DynamicBitmap &other) {
  if(bitmapSize!=other.bitmapSize) {
    free();
    allocate(other.bitsSize);
  }
  for (int i = 0; i < bitmapSize; i++) {
    map[i] = other.map[i];
  }
};
DynamicBitmap::~DynamicBitmap() {
  free();
  if(0) ;
}

void DynamicBitmap::allocate(const size_t &bitsSize_) {
  if(map!= nullptr) free();

  bitsSize=bitsSize_;
  byteSize=BITSET_DIV_ROUND_UP(bitsSize, C_BITS_PER_BYTE);
  bitmapSize=BITSET_DIV_ROUND_UP(byteSize, C_BYTE_PER_BITMAP);

  map=new BitmapType[bitmapSize];
}

void DynamicBitmap::free() {
  if(map== nullptr) {

    bitsSize = 0;
    byteSize = 0;
    bitmapSize = 0;

    delete[] map;
  }
}

void DynamicBitmap::print(std::ostream &ostream) const {
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

void DynamicBitmap::printRaw(std::ostream &ostream) const {
  bool first = true;
  for (size_t i = bitmapSize - 1; i > 0; i--) {
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

}