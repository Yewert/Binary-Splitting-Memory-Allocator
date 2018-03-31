//
// Created by Yewert on 27-Mar-18.
//

#include "MemoryManager.h"
#include <cstdlib>
#include <stdexcept>
#include <iostream>

using namespace std;

void* MemoryManager::allocate(size_t amountOfBytes) {
  MemoryPartDescriptor* targetDescriptor = getTargetDescriptor(amountOfBytes);
  if (targetDescriptor != nullptr) {
    targetDescriptor = getMinimizedDescriptor(amountOfBytes, targetDescriptor);
    return targetDescriptor->getBody().getData();
  }
  return nullptr;
}
MemoryPartDescriptor* MemoryManager::getMinimizedDescriptor(size_t amountOfBytes,
                                                            MemoryPartDescriptor* targetDescriptor) const {
  size_t capacity = ((size_t) 1) << (unsigned char) (targetDescriptor->getLog2OfSize() - 1);
  while ((capacity - sizeof(MemoryPartDescriptor)) >= amountOfBytes) {
    cout << capacity << endl;
    MemoryPartDescriptor* newDescriptor = bisectDescriptor(targetDescriptor, capacity);
    targetDescriptor->setNext(nullptr);
    descriptors[targetDescriptor->getLog2OfSize() - powerOffset] = targetDescriptor;
    targetDescriptor = newDescriptor;
    capacity = ((size_t) 1) << (targetDescriptor->getLog2OfSize() - 1);
  }
  return targetDescriptor;
}
MemoryPartDescriptor* MemoryManager::bisectDescriptor(MemoryPartDescriptor* targetDescriptor, size_t capacity) const {
  MemoryPartDescriptor* newDescriptor = reinterpret_cast<MemoryPartDescriptor*>((void*) targetDescriptor + capacity);
  *newDescriptor = MemoryPartDescriptor((unsigned char) (targetDescriptor->getLog2OfSize() - 1), newDescriptor + 1);
  targetDescriptor->setLog2OfSize((unsigned char) (targetDescriptor->getLog2OfSize() - 1));
  return newDescriptor;
}
MemoryPartDescriptor* MemoryManager::getTargetDescriptor(size_t amountOfBytes) const {
  MemoryPartDescriptor* targetDescriptor = nullptr;
  for (int i = 0; i < log2OfTotalMemory - powerOffset + 1; ++i) {
    if (descriptors[i] != nullptr
        && isBigEnough(amountOfBytes, descriptors[i]->getLog2OfSize())) {
      MemoryPartDescriptor* current;
      MemoryPartDescriptor* previous;
      getLastPairOfDescriptors(i, current, previous);
      if (previous == current) {
        descriptors[i] = nullptr;
      } else {
        previous->getBody().setNext(nullptr);
      }
      current->setDataAddress(sizeof(MemoryPartDescriptor) + reinterpret_cast<void*>(current));
      targetDescriptor = current;
      break;
    }
  }
  return targetDescriptor;
}
bool MemoryManager::isBigEnough(const size_t amountOfBytes, const unsigned char log2OfManagedDataSize) const {
  return amountOfBytes <= ((((size_t) 1) << log2OfManagedDataSize) - sizeof(MemoryPartDescriptor));
}
void MemoryManager::getLastPairOfDescriptors(const int targetPower,
                                             MemoryPartDescriptor*&current,
                                             MemoryPartDescriptor*&previous) const {
  current = descriptors[targetPower];
  previous = descriptors[targetPower];
  while (current->getBody().getNext() != nullptr) {
    previous = current;
    current = current->getBody().getNext();
  }
}

void MemoryManager::free(void* address) {
  //TODO: merge of adjacent blocks
  auto correspondingDescriptor = reinterpret_cast<MemoryPartDescriptor*>(address - sizeof(MemoryPartDescriptor));
  correspondingDescriptor->setNext(nullptr);
  auto current = descriptors[correspondingDescriptor->getLog2OfSize() - powerOffset];
  if (current == nullptr) {
    descriptors[correspondingDescriptor->getLog2OfSize() - powerOffset] = correspondingDescriptor;
    return;
  }
  while (current->getBody().getNext() != nullptr) {
    current = current->getBody().getNext();
  }
  current->setNext(correspondingDescriptor);
}

MemoryManager::MemoryManager(unsigned char log2OfTotalMemory) {
  MemoryManager::log2OfTotalMemory = log2OfTotalMemory;
  descriptors = new MemoryPartDescriptor* [log2OfTotalMemory - powerOffset + 1];
  for (int i = 0; i < log2OfTotalMemory - powerOffset; ++i) {
    descriptors[i] = nullptr;
  }
  size_t size = (size_t)1 << log2OfTotalMemory;
  startOfMemoryBlock = malloc(size);
  if (startOfMemoryBlock == nullptr)
    throw length_error("not enough space to allocate");
  descriptors[log2OfTotalMemory - powerOffset] = new(startOfMemoryBlock) MemoryPartDescriptor(log2OfTotalMemory);
}
void* MemoryManager::getStartOfMemoryBlock() const {
  return startOfMemoryBlock;
}
MemoryManager::~MemoryManager() {
  delete[] descriptors;
}