#include <iostream>
#include "MemoryManager.h"

using namespace std;

int main() {
  auto manager = MemoryManager(10);
  auto address = manager.allocate(490);
  auto address1 = manager.allocate(490);
  auto address2 = manager.allocate(490);
  cout << address << endl;
  cout << address1 << endl;
  cout << address2 << endl;
  manager.free(address);
  address2 = manager.allocate(490);
  cout << address1 << endl;
}
