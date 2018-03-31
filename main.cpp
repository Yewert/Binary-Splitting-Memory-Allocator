#include <iostream>
#include "MemoryManager.h"

using namespace std;

int main() {
//  auto x = malloc(100);
//  auto b = new(x) MemoryManager(10);
//  b->~MemoryManager();
//  delete b;
  auto manager = MemoryManager(10);
  auto address = manager.allocate(100);
  cout << address << endl;
  cout << manager.getStartOfMemoryBlock() << endl;
  manager.free(address);
}
