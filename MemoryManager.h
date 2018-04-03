//
// Created by Yewert on 27-Mar-18.
//

#ifndef MEMORYALLOCATOR_MEMORYMANAGER_H
#define MEMORYALLOCATOR_MEMORYMANAGER_H

#include <cstddef>
#include "MemoryPartDescriptor.h"

static constexpr unsigned char findLowestPowerOf2GreaterThan(size_t number){
  for (unsigned char i = 0; i < sizeof(size_t) * 8; ++i) {
    if ((((unsigned long) 1) << i) >= number)
      return i;
  }
  return 0;
}

class MemoryManager {
 private:
  static constexpr unsigned char powerOffset = findLowestPowerOf2GreaterThan(sizeof(MemoryPartDescriptor) + 1);
  void * startOfMemoryBlock;
  unsigned char log2OfTotalMemory;
  MemoryPartDescriptor** descriptorTable;
  void getLastPairOfDescriptorsInTable(int targetPower,
                                       MemoryPartDescriptor*&current,
                                       MemoryPartDescriptor*&previous) const;
  MemoryPartDescriptor *getTargetDescriptor(size_t amountOfBytes) const;
  bool isBigEnough(size_t amountOfBytes, unsigned char log2OfManagedDataSize) const;
  MemoryPartDescriptor* getMinimizedDescriptor(size_t amountOfBytes, MemoryPartDescriptor* targetDescriptor);
  MemoryPartDescriptor* bisectDescriptor(MemoryPartDescriptor* targetDescriptor, size_t capacity);
  MemoryPartDescriptor* getLastDescriptorInTable(MemoryPartDescriptor* current) const;
  void addToDescriptorTable(MemoryPartDescriptor* newRecord);
  void removeDescriptorFromTheTable(MemoryPartDescriptor* outdatedRecord);
 public:
  void *getStartOfMemoryBlock() const;
  void *allocate(size_t amountOfBytes);
  void freeSpace(void* address);
  explicit MemoryManager(unsigned char log2OfTotalMemory);
  ~MemoryManager();
};

#endif //MEMORYALLOCATOR_MEMORYMANAGER_H
