//
// Created by qiu on 17/06/18.
//

#include "utils/bitmap_fixed.h"
#include <iostream>
#include <vector>

using namespace patternmatching;

int main(int argc, char **argv) {
  FixedBitmap<64> fixedBitmap;

  std::cout << "initial print" << std::endl;
  fixedBitmap.print();

  std::cout << "fill" << std::endl;
  fixedBitmap.fill();
  fixedBitmap.print();

  std::cout << "erase 12 24 32 48" << std::endl;
  std::vector<size_t> eraseVector = {12, 24, 32, 48};
  for (const auto &it : eraseVector) { fixedBitmap.erase(it); }
  fixedBitmap.print();

  std::cout << "size" << std::endl;
  std::cout << fixedBitmap.size()<< std::endl;

  std::cout << "empty" << std::endl;
  std::cout << fixedBitmap.empty()<< std::endl;

  std::cout << "clear" << std::endl;
  fixedBitmap.clear();
  fixedBitmap.print();

  std::cout << "empty" << std::endl;
  std::cout << fixedBitmap.empty()<< std::endl;

  std::cout << "insert 0 9 13 15 25 33 49" << std::endl;
  std::vector<size_t> insertVector = {0, 9, 13, 15, 25, 33, 49};
  for (const auto &it : insertVector) { fixedBitmap.insert(it); }
  fixedBitmap.print();

  std::cout << "insert 0 9 13 15 25 33 49" << std::endl;
  std::vector<size_t> insertVector2 = {52,53,54,55,56,57,58};
  fixedBitmap.insert(insertVector2.cbegin(), insertVector2.cend());
  fixedBitmap.print();

  std::cout << "isIn 9" << std::endl;
  std::cout << fixedBitmap.isIn(9) << std::endl;

  std::cout << "find 9 -> next" << std::endl;
  auto it9 = fixedBitmap.find(9);
  it9++;
  std::cout << *it9 << std::endl;

  std::cout << "isIn 8" << std::endl;
  std::cout << fixedBitmap.isIn(8) << std::endl;

  std::cout << "find 8 == end" << std::endl;
  auto it8 = fixedBitmap.find(8);
  std::cout << (it8==fixedBitmap.cend()) << std::endl;


  std::cout << "print Raw" << std::endl;
  fixedBitmap.printRaw();


  std::cout << "insert atomic 1" << std::endl;
  fixedBitmap.insertAtomic(1);
  fixedBitmap.printRaw();

  std::cout << "isIn atomic 1" << std::endl;
  std::cout << fixedBitmap.isInAtomic(1) << std::endl;

  return 0;
}

