//
// Created by Yewert on 27-Mar-18.
//

#ifndef MEMORYALLOCATOR_MEMORYPARTDESCRIPTOR_H
#define MEMORYALLOCATOR_MEMORYPARTDESCRIPTOR_H

#include "Selector.h"

class MemoryPartDescriptor {

 private:
  Selector body{
      Selector()
  };
  bool isFree{true};
  unsigned char log2OfSize{};
 public:
  void setLog2OfSize(unsigned char log2OfSize);
  unsigned char getLog2OfSize() const;
  bool IsFree() const;
  void setNext(MemoryPartDescriptor* next);
  MemoryPartDescriptor* getNext();
  void setDataAddress(void* data);
  void* getDataAddress();
  MemoryPartDescriptor() = default;
  explicit MemoryPartDescriptor(unsigned char log2OfSize);
};

#endif //MEMORYALLOCATOR_MEMORYPARTDESCRIPTOR_H