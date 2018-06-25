//
// Created by qiu on 17/06/18.
//

#include "utils/bitmap_dynamic.h"
#include <iostream>
#include <vector>

using namespace patternmatching;

int main(int argc, char **argv) {
  DynamicBitmap dynamicBitmap;
  dynamicBitmap.allocate(64);

  std::cout << "initial print" << std::endl;
  dynamicBitmap.print();

  std::cout << "fill" << std::endl;
  dynamicBitmap.fill();
  dynamicBitmap.print();

  std::cout << "erase 12 24 32 48" << std::endl;
  std::vector<size_t> eraseVector = {12, 24, 32, 48};
  for (const auto &it : eraseVector) { dynamicBitmap.erase(it); }
  dynamicBitmap.print();

  std::cout << "size" << std::endl;
  std::cout << dynamicBitmap.size()<< std::endl;

  std::cout << "empty" << std::endl;
  std::cout << dynamicBitmap.empty()<< std::endl;

  std::cout << "clear" << std::endl;
  dynamicBitmap.clear();
  dynamicBitmap.print();

  std::cout << "empty" << std::endl;
  std::cout << dynamicBitmap.empty()<< std::endl;

  std::cout << "insert 0 9 13 15 25 33 49" << std::endl;
  std::vector<size_t> insertVector = {0, 9, 13, 15, 25, 33, 49};
  for (const auto &it : insertVector) { dynamicBitmap.insert(it); }
  dynamicBitmap.print();

  std::cout << "insert 0 9 13 15 25 33 49" << std::endl;
  std::vector<size_t> insertVector2 = {52,53,54,55,56,57,58};
  dynamicBitmap.insert(insertVector2.cbegin(), insertVector2.cend());
  dynamicBitmap.print();

  std::cout << "isIn 9" << std::endl;
  std::cout << dynamicBitmap.isIn(9) << std::endl;

  std::cout << "find 9 -> next" << std::endl;
  auto it9 = dynamicBitmap.find(9);
  it9++;
  std::cout << *it9 << std::endl;

  std::cout << "isIn 8" << std::endl;
  std::cout << dynamicBitmap.isIn(8) << std::endl;

  std::cout << "find 8 == end" << std::endl;
  auto it8 = dynamicBitmap.find(8);
  std::cout << (it8==dynamicBitmap.cend()) << std::endl;


  std::cout << "print Raw" << std::endl;
  dynamicBitmap.printRaw();


  std::cout << "insert atomic 1" << std::endl;
  dynamicBitmap.insertAtomic(1);
  dynamicBitmap.printRaw();

  std::cout << "isIn atomic 1" << std::endl;
  std::cout << dynamicBitmap.isInAtomic(1) << std::endl;

  std::cout << "erase atomic 1" << std::endl;
  dynamicBitmap.eraseAtomic(1);
  dynamicBitmap.printRaw();

  std::cout << "isIn atomic 1" << std::endl;
  std::cout << dynamicBitmap.isInAtomic(1) << std::endl;

  std::cout << "fill" << std::endl;
  dynamicBitmap.fill();
  dynamicBitmap.printRaw();

  return 0;
}

