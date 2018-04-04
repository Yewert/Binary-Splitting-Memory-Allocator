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
//  manager.printSummary();
//  manager.freeSpace(address1);
//  manager.freeSpace(address2);
//  address3 = manager.allocate(4900);
//  manager.printSummary();
//  manager.freeSpace(address3);
//  auto address4 = manager.allocate(1000);
//  auto address5 = manager.allocate(2000);
//  manager.printSummary();
//  auto address6 = manager.allocate(500000);
  manager.printSummary();
  auto x = new void* [10000];
  for (int i = 0; i < 10000; ++i) {
    x[i] = manager.allocate(1);
  }
  manager.printSummary();
  for (int j = 0; j < 10000; ++j) {
    manager.freeSpace(x[j]);
//    manager.printSummary();
  }
  manager.printSummary();
  cout << "yo" << endl;
  system("pause");
}
