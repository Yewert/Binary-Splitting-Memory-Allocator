//
// Created by Yewert on 27-Mar-18.
//

#ifndef MEMORYALLOCATOR_SELECTOR_H
#define MEMORYALLOCATOR_SELECTOR_H

#include "MemoryPartDescriptor.h"

class MemoryPartDescriptor;

union Selector {
 private:
  void* data;
  MemoryPartDescriptor* next;
 public:
  void* getData() const;
  void setData(void* data);
  MemoryPartDescriptor* getNext() const;
  void setNext(MemoryPartDescriptor* next);
};

#endif //MEMORYALLOCATOR_SELECTOR_H
