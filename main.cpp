#include <iostream>
#include "MemoryManager.h"

using namespace std;

int main() {
  system("pause");
  auto manager = MemoryManager(30);
  auto address = manager.allocate(4900);
  auto address1 = manager.allocate(2000);
  auto address2 = manager.allocate(2000);
  auto address3 = manager.allocate(4900);
  manager.printSummary();
  system("pause");
  auto x = new void* [10000];
  for (int i = 0; i < 10000; ++i) {
    x[i] = manager.allocate(1);
  }
  manager.printSummary();
  system("pause");
  for (int j = 0; j < 10000; ++j) {
    manager.freeSpace(x[j]);
  }
  manager.printSummary();
  system("pause");
}
