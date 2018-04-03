#include <iostream>
#include "MemoryManager.h"

using namespace std;

int main() {
  auto manager = MemoryManager(10);
  auto address = manager.allocate(490);
  auto address1 = manager.allocate(200);
  auto address2 = manager.allocate(200);
  auto address3 = manager.allocate(490);
  cout << address << endl;
  cout << address1 << endl;
  cout << address2 << endl;
  cout << address3 << endl;
  cout << "-------------" << endl;
  manager.freeSpace(address1);
  manager.freeSpace(address2);
  address3 = manager.allocate(490);
  cout << address3 << endl;
  manager.freeSpace(address3);
  cout << "-------------" << endl;
  auto address4 = manager.allocate(100);
  auto address5 = manager.allocate(200);
  auto address6 = manager.allocate(100);
  cout << address4 << endl;
  cout << address5 << endl;
  cout << address6 << endl;
}
